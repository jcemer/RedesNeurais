// Developed by Eduardo Wisnieski Basso - mailto:ewbasso@inf.ufrgs.br
// NO WARRANTY is given over this source code. You are free to use or change
// this source code as you wish.
//------------------------------------------------------------------------------
#ifndef SOCK_CPP
#define SOCK_CPP

#include "sock.hpp"

#ifdef _WIN32   // Win32
#include <winsock2.h>
#include <mem.h>
#else           // Linux
#include <memory.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#endif

//------------------------------------------------------------------------------
namespace sock {

/// Initializes the socket instances counter.
int     sock::instanceCount = 0;

/// Default constructor.
//------------------------------------------------------------------------------
sock::sock( ) {

    #ifdef _WIN32       // Win32
    WSADATA wsaData;    // Used during winsock library.
    #endif

    // Initializes socket data.
    connectStatus = connStatusClosed;
    socketHandler = SOCK_INVALID;

    // Count the socket instances.
    if ( instanceCount <= 0 ) {

        instanceCount = 0;

        #ifdef _WIN32   // Win32
        // Initializes the winsock library.
        if ( WSAStartup(0x0202,&wsaData) != 0 ) {
            throw std::runtime_error( "Initializing the winsock library" );
        }
        #endif
    }
    instanceCount++;
}
//------------------------------------------------------------------------------

/// Copy constructor.
//------------------------------------------------------------------------------
sock::sock( sock &_socket )
{
    sock( );

    // Copies the internal controls.
    connectStatus = _socket.connectStatus;
    socketHandler = _socket.socketHandler;

    // Closes the original instance.
    _socket.connectStatus = connStatusClosed;
    _socket.socketHandler = SOCK_INVALID;
}
//------------------------------------------------------------------------------

/// Copy operator.
//------------------------------------------------------------------------------
sock& sock::operator=( sock &_socket ) {

    // Copies the internal controls.
    connectStatus = _socket.connectStatus;
    socketHandler = _socket.socketHandler;

    // Closes the original instance.
    _socket.connectStatus = connStatusClosed;
    _socket.socketHandler = SOCK_INVALID;

    return *this;
}
//------------------------------------------------------------------------------

/// Destructor.
//------------------------------------------------------------------------------
sock::~sock( ) {

    // Closes the socket.
    this->close();

    // Count the socket instances.
    instanceCount--;
    if ( instanceCount <= 0 ) {

        // Releases the winsock library.
        #ifdef _WIN32   // Win32
        WSACleanup();
        #endif
        instanceCount = 0;
    }
}
//------------------------------------------------------------------------------

/// Gets the status of the socket connection.
//------------------------------------------------------------------------------
connStatus sock::getConnStatus( ) const {

    return connectStatus;
}
//------------------------------------------------------------------------------

/// Gets the socket handler.
//------------------------------------------------------------------------------
int32 sock::getHandler( ) const {

    return socketHandler;
}
//------------------------------------------------------------------------------

/// Sets the buffer size for the receiving data.
//------------------------------------------------------------------------------
bool sock::setRecvBuffer( int _size ) {

    if ( socketHandler == SOCK_INVALID ) {
        return false;
    }
    if ( _size <= 0 ) {
        return false;
    }

    // Configures and tests.
    if ( ! setsockopt( socketHandler, SOL_SOCKET, SO_RCVBUF,
                       (char*)&_size, sizeof( _size ) ) ) {
        throw std::runtime_error( "Setting the receive buffer size" );
    }
    if ( this->getRecvBuffer() != _size ) {
        return false;
    }

    return true;
}
//------------------------------------------------------------------------------

/// Gets the buffer size for the receiving data.
//------------------------------------------------------------------------------
int sock::getRecvBuffer( ) const {

    int32       size;       // Buffer size.
    socklen_t   temp;       // Temporary size of the variable.

    if ( socketHandler == SOCK_INVALID ) {
        return 0;
    }

    // Gets buffer size.
    size = 0;
    temp = sizeof( size );
    if ( ! getsockopt( socketHandler, SOL_SOCKET, SO_RCVBUF,
                       (char*)&size, &temp ) ) {
        throw std::runtime_error( "Getting the receive buffer size" );
    }

    return size;
}
//------------------------------------------------------------------------------

/// Sets the buffer size for the sending data.
//------------------------------------------------------------------------------
bool sock::setSendBuffer( int _size ) {

    if ( socketHandler == SOCK_INVALID ) {
        return false;
    }
    if ( _size <= 0 ) {
        return false;
    }

    // Configures and tests.
    if ( ! setsockopt( socketHandler, SOL_SOCKET, SO_SNDBUF,
                       (char*)&_size, sizeof( _size ) ) ) {
        throw std::runtime_error( "Setting the send buffer size" );
    }
    if ( this->getSendBuffer() != _size ) {
        return false;
    }

    return true;
}
//------------------------------------------------------------------------------

/// Gets the buffer size for the sending data.
//------------------------------------------------------------------------------
int sock::getSendBuffer( ) const {

    int32       size;       // Buffer size.
    socklen_t   temp;       // Temporary size of the variable.

    if ( socketHandler == SOCK_INVALID ) {
        return 0;
    }

    // Gets buffer size.
    size = 0;
    temp = sizeof( size );
    if ( ! getsockopt( socketHandler, SOL_SOCKET, SO_SNDBUF,
                       (char*)&size, &temp ) ) {
        throw std::runtime_error( "Getting the send buffer size" );
    }

    return size;
}
//------------------------------------------------------------------------------

/// Gets the socket local name.
//------------------------------------------------------------------------------
bool sock::getLocalName( uint32 &_address, uint16 &_port ) const {

    struct sockaddr name;       // Socket name.
    socklen_t       nameSize;   // Socket name size.


    if ( socketHandler == SOCK_INVALID ) {
        return false;
    }

    // Gets the name.
    nameSize = sizeof( struct sockaddr );
    if ( ! getsockname( socketHandler, &name, &nameSize ) ) {
        return false;
    }

    // Data convertion.
    _address = *(uint32*) ( name.sa_data + 2 );
    _port = ntohs( *(uint16*) ( name.sa_data ) );

    return true;
}
//------------------------------------------------------------------------------

/// Gets the socket remote name.
//------------------------------------------------------------------------------
bool sock::getRemoteName( uint32 &_address, uint16 &_port ) const {

    struct sockaddr name;       // Socket name.
    socklen_t       nameSize;   // Socket name size.

    if ( socketHandler == SOCK_INVALID ) {
        return false;
    }

    // Gets the name.
    nameSize = sizeof( struct sockaddr );
    if ( ! getpeername( socketHandler, &name, &nameSize ) ) {
        return false;
    }

    // Data convertion.
    _address = *(uint32*) ( name.sa_data + 2 );
    _port = ntohs( *(uint16*) ( name.sa_data ) );

    return true;
}
//------------------------------------------------------------------------------

/// Parse an address string.
//------------------------------------------------------------------------------
uint32 sock::resolveAddress( const char *_address ) {

    uint32          ip;     // IP address.
    struct hostent *host;   // List of localized hosts.

    // Try to convert as an IP address.
    ip = inet_addr( _address );
    if ( ip != INADDR_NONE ) {
        return ip;
    }

    // Try to resolve the IP address by the hostname.
    host = gethostbyname( _address );
    if ( host == NULL ) {
        return INADDR_NONE;
    }
    switch ( host->h_length ) {
        case 1:
            ip = *(uint8*) ( host->h_addr_list[0] );
        break;
        case 2:
            ip = *(uint16*) ( host->h_addr_list[0] );
        break;
        case 4:
            ip = *(uint32*) ( host->h_addr_list[0] );
        break;
        default:
            ip = INADDR_NONE;
    }

    return ip;
}
//------------------------------------------------------------------------------

/// Gets a string representation of an IP address.
//------------------------------------------------------------------------------
const char* sock::addressToString( uint32 _address ) {

    struct in_addr  tempAddr;       // Temporary address structure.

    #ifdef _WIN32
    tempAddr.S_un.S_addr = _address;
    #else
    tempAddr.s_addr = _address;
    #endif
    return inet_ntoa( tempAddr );
}
//------------------------------------------------------------------------------

/// Connects the socket to a server.
//------------------------------------------------------------------------------
bool sock::connect( uint32 _address, uint16 _port ) {

    int                 error;      // Socket calls returning value.
    struct sockaddr_in  addressData;// Server address.
    //int                 dataAux;    // Temporary getting data.

    // Closes the current connection.
    this->close();

    // Creates a new socket.
    socketHandler = ::socket( AF_INET, SOCK_STREAM, 0 );    //SOCK_DGRAM,0);
    if ( socketHandler == SOCK_INVALID ) {
        throw std::runtime_error( "Creating socket" );
    }

    // Build the server address.
	addressData.sin_family = AF_INET;
	addressData.sin_port = htons( _port );
    addressData.sin_addr.s_addr = _address;
    memset( &addressData.sin_zero, 0, 8 );

    // Connects to the server.
    error = ::connect( socketHandler, (struct sockaddr *) &addressData,
                       sizeof( addressData ) );
	if ( error < 0 ) {
        this->close();
        return false;
    }

    connectStatus = connStatusClient;

    return true;
}
//------------------------------------------------------------------------------

/// Puts the socket listening the specified port.
//------------------------------------------------------------------------------
bool sock::listen( uint16 _port, int _connectBuf ) {

    int                 error;      // Socket calls returning value.
    struct sockaddr_in  addressData;// Listen socket address.

    // Closes the current connection.
    this->close();

    // Creates a new socket.
    socketHandler = ::socket( AF_INET, SOCK_STREAM, 0 );    //SOCK_DGRAM,0);
    if ( socketHandler == SOCK_INVALID ) {
        throw std::runtime_error( "Creating socket" );
    }

    // Builds the listen address.
	addressData.sin_family = AF_INET;
	addressData.sin_port = htons( _port );
	addressData.sin_addr.s_addr = htonl( INADDR_ANY );
    memset( &addressData.sin_zero, 0, 8 );

    // Binds the socket to the interface (address and port).
	error = ::bind( socketHandler, (struct sockaddr *) &addressData,
                    sizeof( addressData ) );
	if ( error < 0 ) {
        this->close();
        return false;
	}

    // Listen the port.
	error = ::listen( socketHandler, _connectBuf );
	if ( error < 0 ) {
        this->close();
        throw std::runtime_error( "Listening port" );
	}

    connectStatus = connStatusServer;

    return true;
}
//------------------------------------------------------------------------------

/// Accepts a client socket connection to the server.
//------------------------------------------------------------------------------
bool sock::accept( const sock &_server ) {

    // Closes the current connection and checks the server.
    this->close();
    if ( _server.connectStatus != connStatusServer ) {
        return false;
    }

    // Expects for a connection request.
    socketHandler = ::accept( _server.socketHandler, NULL, NULL );
    if ( socketHandler == SOCK_INVALID ) {
        this->close();
        throw std::runtime_error( "Accepting socket connection" );
    }

    connectStatus = connStatusRemote;

    return true;
}
//------------------------------------------------------------------------------

/// Closes the socket.
//------------------------------------------------------------------------------
void sock::close( ) {

    // Closes the socket connection.
    if ( socketHandler != SOCK_INVALID ) {
        #ifdef _WIN32
        closesocket( socketHandler );
        #else
        ::close( socketHandler );
        #endif
        socketHandler = SOCK_INVALID;
    }

    connectStatus = connStatusClosed;
}
//------------------------------------------------------------------------------

/// Sends data through the socket.
//------------------------------------------------------------------------------
bool sock::send( uchar *_data, int _size ) {

    int         error;      // Send returning value.

    if ( socketHandler == SOCK_INVALID ) {
        return false;
    }

    // Checks the data array.
    if ( _data == NULL ) {
        return false;
    }

    // Send data.
    #ifdef _WIN32
    error = ::send( socketHandler, (char FAR*) _data, _size, 0 );
    #else
    error = ::send( socketHandler, (void*) _data, _size, 0 );
    #endif
    if ( error != _size ) {
        return false;
    }

    return true;
}
//------------------------------------------------------------------------------

/// Receives data from the socket.
//------------------------------------------------------------------------------
bool sock::recv( uchar *_data, int &_size, uint32 _timeout ) {

    int             error;          // Recv returning value.
    int             toRead;         // Bytes to be read.

    toRead = _size;
    _size =0;

    if ( socketHandler == SOCK_INVALID ) {
        return false;
    }

    // Checks parameters.
    if ( toRead <= 0 )
    {
        return false;
    }
    if ( _data == NULL ) {
        return false;
    }

    // Waits for a reading pending.
    if ( !( this->select( true, false, _timeout ) & 1 ) ) {
        return false;
    }

    // Reads data from the socket connection.
    #ifdef _WIN32
    error = ::recv( socketHandler, (char FAR*) _data, toRead, 0 );
    #else
    error = ::recv( socketHandler, (void*) _data, toRead, 0 );
    #endif
    if ( error < 0 ) {
        this->close();
        return false;
    }

    // Checks end of connection.
    if ( error == 0 ) {
        this->close();
        return false;
    }

    // Sets read bytes.
    _size = error;

    return true;
}
//------------------------------------------------------------------------------

/// Verifies the status of the socket connection, looking for read and write
/// pending actions.
//------------------------------------------------------------------------------
int sock::select( bool _read, bool _write, uint32 _timeout ) {

    fd_set          readfds;        // List of reading sockets to be checked.
    fd_set          writefds;       // List of writing sockets to be checked.
    int32           error;          // Select and recv returning values.
    struct timeval  timeoutStruct;  // Timeout structure.
    struct timeval *timeoutPtr;     // Pointer to the timeout structure.
    int             result;         // Combined result for the select action.

    timeoutPtr = NULL;

    if ( socketHandler == SOCK_INVALID ) {
        return 0;
    }

    // Builds the checking lists for select.
    FD_ZERO( &readfds );
    FD_ZERO( &writefds );
    if ( _read ) {
        FD_SET( socketHandler, &readfds );
    }
    if ( _write ) {
        FD_SET( socketHandler, &writefds );
    }

    // Sets the timeout structure and the timeout structure pointer. The pointer
    // is NULL if no timeout is desired (infinite waiting).
    if ( _timeout < 0xFFFFFFFF )
    {
        timeoutStruct.tv_sec = _timeout / 1000;
        timeoutStruct.tv_usec = ( _timeout % 1000 ) * 1000;
        timeoutPtr = &timeoutStruct;
    }

    // Looks for reading and/or writing pendings. First parameter is not used
    // on win32.
    error = ::select( socketHandler + 1, &readfds, &writefds, NULL, timeoutPtr );
    if ( error < 0 ) {
        return false;
    }

    // Combines the result.
    result = 0;
    if ( FD_ISSET( socketHandler, &readfds ) ) {
        result |= 1;
    }
    if ( FD_ISSET( socketHandler, &writefds ) ) {
        result |= 2;
    }

    return result;
}
//------------------------------------------------------------------------------

}; // namespace sock.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#endif // SOCK_CPP
