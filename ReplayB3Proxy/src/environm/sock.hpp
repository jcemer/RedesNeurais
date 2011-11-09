// Developed by Eduardo Wisnieski Basso - http://www.inf.ufrgs.br/~ewbasso 
// NO WARRANTY is given over this source code. You are free to use or change
// this source code as you wish. Contact-me through mailto:ewbasso@inf.ufrgs.br.
//------------------------------------------------------------------------------
#ifndef SOCK_HPP
#define SOCK_HPP

#include <stdexcept>
#include "basetp.hpp"

/// Classes to work with socket interfaces.  No source code changes are needed
/// in order to port between Linux and Windows. 
//------------------------------------------------------------------------------
namespace sock {

/// Uses basetp namespace.
using namespace baseTp;

#define SOCK_INVALID        -1      ///< Invalid handler for the socket.

#ifdef _WIN32
typedef int     socklen_t;
#endif

/// Status of the connection for a socket. The socket can be closed; it can be a
/// server (by listen), if it is waiting for connections; it can be a client (by
/// connect), if it has been connected to a server; and it can be a remote (by
/// accept), if it is a local instance of a remote socket that has been
/// connected to this host.
enum connStatus { connStatusClosed=0, connStatusServer=1,
                  connStatusClient=2, connStatusRemote=3 };

/// Implementation of a socket interface that can work as a server, a client, or
/// an remote socket. A server socket listens a port and receives connection
/// requests. A client socket connects to a server and, after the connection is
/// accepted, sends and receives messages. A remote socket is the remote part of
/// a connection. It is like a client socket, but it accepts the connection from
/// other socket instead of connecting to other one.
//------------------------------------------------------------------------------
class sock
{
public:
    /// Default constructor. It initializes the internal data and loads the
    /// socket library, if it needs to.
    /// @throw runtime_error if it fails loading the socket library.
    sock( );

    /// Copy constructor. It creates an identical copy for the socket. The
    /// original socket is closed, but the connection continues existing becouse
    /// the new instance needs that.
    /// @param _socket: Original socket to be copied and closed.
    sock( sock &_socket );

    /// Copy operator. It creates an identical copy for the socket. The
    /// original socket is closed, but the connection continues existing becouse
    /// the new instance needs that.
    /// @param _socket: Original socket to be copied and closed.
    /// @return Current object's reference.
    sock& operator=( sock &_socket );

    /// Destructor. It forces the socket to be destroyed and releases the
    /// library, if it needs to.
    ~sock( );

    /// Gets the status of the socket connection.
    /// @return The indicative value for the connection status. It is closed if
    /// the socket is not connected, listening, or accepted.
    /// @sa connStatus.
    connStatus getConnStatus( ) const;

    /// Gets the socket handler.
    /// @return The socket handler, or SOCK_INVALID if it is closed.
    int32 getHandler( ) const;

    /// Sets the buffer size for the receiving data.
    /// @param _size: Size (in bytes) of the receive buffer. Must be > 0.
    /// @return True if success, or false if the socket is closed or _size is
    /// invalid.
    /// @throw runtime_error if the socket library method fails.
    bool setRecvBuffer( int _size );

    /// Gets the buffer size for the receiving data.
    /// @return The receive buffer size (in bytes) if success, or ZERO if the
    /// socket is closed.
    /// @throw runtime_error if the socket library method fails.
    int getRecvBuffer( ) const;

    /// Sets the buffer size for the sending data.
    /// @param _size: Size (in bytes) of the send buffer. Must be > 0.
    /// @return True if success, or false if the socket is closed or _size is 
    /// invalid.
    /// @throw runtime_error if the socket library method fails.
    bool setSendBuffer( int _size );

    /// Gets the buffer size for the sending data.
    /// @return The send buffer size (in bytes) if success, or ZERO if the 
    /// socket is closed.
    /// @throw runtime_error if the socket library method fails.
    int getSendBuffer( ) const;

    /// Gets the socket local name.
    /// @param _address: Receives the socket local IP address.
    /// @param _port: Receives the socket's local TCP port in the host byte 
    /// order.
    /// @return True if success, or false if the socket is closed.
    /// @throw runtime_error if the socket library method fails.
    bool getLocalName( uint32 &_address, uint16 &_port ) const;

    /// Gets the socket remote name.
    /// @param _address: Receives the socket remote IP address.
    /// @param _port: Receives the socket's remote TCP port in the host byte 
    /// order.
    /// @returns True if success, or false if the socket is closed.
    /// @throw runtime_error if the socket library method fails.
    bool getRemoteName( uint32 &_address, uint16 &_port ) const;

    /// Parses an address string. It converts an IP address string or a host name
    /// string to an IP number.
    /// @param _address: Address string to be parsed. It can be an IP address
    /// ( like "127.0.0.1" ) or a host name ( like "localhost" ).
    /// @return The IP number in the host byte order if success, or INADDR_NONE
    /// if address is NULL or it is an unknown address.
    static uint32 resolveAddress( const char *_address );

    /// Gets a string representation of an IP address.
    /// @param _address: IP address to be converted.
    /// @return The string with the IP address representation if success, or 
    /// NULL if it fails.
    static const char* addressToString( uint32 _address );

    /// Connects the socket to a server. The socket becomes a client socket.
    /// @param _address: Remote IP address of the server.
    /// @param _port: Remote TCP port where the server is listening.
    /// @return True if success, or false if the server cannot be found.
    /// @throw runtime_error if the socket library method fails.
    bool connect( uint32 _address, uint16 _port );

    /// Puts the socket listening the specified port. The socket becomes a
    /// server socket.
    /// @param _port: Local TCP port to listen.
    /// @param _connectBuf: Maximum size of the waiting queue ( default = 5 ).
    /// @return True if success, or false if the port cannot be listened.
    /// @throw runtime_error if the socket library method fails.
    bool listen( uint16 _port, int _connectBuf = 5 );

    /// Accepts a client socket connection to the server. The socket becomes a
    /// remote socket.
    /// @param _server: Server that received the connection request.
    /// @return True if success, or false if the server is closed or the 
    /// connection cannot be accepted.
    /// @throw runtime_error if the socket library method fails.
    bool accept( const sock &_server );

    /// Closes the socket.
    void close( );

    /// Sends data through the socket. It has to be a client or remote socket.
    /// @param _data: Array of data to be sent through the socket connection.
    /// @param _size: Size (in bytes) of the array of data. Must be > 0.
    /// @return True if success, or false if the socket is closed, the array is
    /// invalid, or the system call fails.
    bool send( uchar *_data, int _size );

    /// Receives data from the socket. Receiving is allowed only for client and
    /// remote sockets. On Linux, the timeout control is done explicitly.
    /// @param _data: Array that will receive data.
    /// @param _size: Size (in bytes) of the array of data. Must be > 0.
    /// @param _timeout: Timeout (in milliseconds) to stop waiting data. The 
    /// receive method becomes immediate if _timeout is ZERO. The waiting is not 
    /// stopped if _timeout is 0xffffffff. Default is 0xffffffff.
    /// @return True if success, or false if the socket is closed, the array is
    /// invalid, the timeout expires, or the system call fails.
    bool recv( uchar *_data, int &_size, uint32 _timeout=0xFFFFFFFF );

    /// Verifies the status of the socket connection, looking for read and write
    /// pending actions. A server can look for connection requests by calling
    /// select() for read pending actions.
    /// @param _read: Says that it is looking for a read pending.
    /// @param _write: Says that it is looking for a read pending.
    /// @param _timeout: Timeout (in milliseconds) to stop waiting data. The 
    /// select method becomes immediate if _timeout is ZERO. The waiting is 
    /// not stopped if _timeout is 0xffffffff. Default is 0xffffffff.
    /// @return ONE if a read pending is detected, TWO if a write pending is
    /// detected, THREE if both are detected, or ZERO if no pending is detected.
    /// @return ZERO if the socket is closed, the timeout expires, or the system
    /// call fails.
    int select( bool _read, bool _write, uint32 _timeout=0xFFFFFFFF );

protected:
    static
    int         instanceCount;  ///< Number of socket instances.
    connStatus  connectStatus;  ///< Type of the socket connection.
    int32       socketHandler;  ///< Socket handler.
};
//------------------------------------------------------------------------------



/// Sends a structure through the socket. Sending is allowed only for
/// client and remote sockets.
/// @param _data: Structure to be sent through the socket connection.
/// @return True if success, or false if the socket is closed or the system
/// call fails.
//------------------------------------------------------------------------------
template <class type>
bool sendStruct( sock &_sock, type &_data ) {

    // Sends the structure as an array of N bytes. N is the structure size.
    return _sock.send( ( uchar* ) &_data, sizeof( _data ) );
}
//------------------------------------------------------------------------------

/// Receives a structure from the socket. Receiving is allowed only for
/// client and remote sockets.
/// @param _data: Structure to be received from the socket connection.
/// @param _timeout: Timeout (in milliseconds) to stop waiting data. The
/// receive method becomes immediate if _timeout is ZERO. The waiting is not
/// stopped if _timeout is 0xffffffff. Default is 0xffffffff.
/// @return True if success, or false if the timeout expires the socket is
/// closed, the system call fails, or it receives less bytes than expected.
//------------------------------------------------------------------------------
template <class type>
bool recvStruct( sock &_sock, type &_data, uint32 _timeout=0xffffffff ) {

    int     size;
    bool    result;

    // Gets the structure size. How many bytes to expect for.
    size = sizeof( _data );

    // Receives the structure as an array of bytes.
    result = _sock.recv( ( uchar* ) &_data, size, _timeout );

    // It also fails if receives few bytes than expected.
    return ( result && ( size == sizeof( _data ) ) );
}
//------------------------------------------------------------------------------

}; // namespace sock.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#endif // SOCK_HPP
