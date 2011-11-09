// Developed by Eduardo Wisnieski Basso - http://www.inf.ufrgs.br/~ewbasso 
// NO WARRANTY is given over this source code. You are free to use or change
// this source code as you wish. Contact-me through mailto:ewbasso@inf.ufrgs.br.
//------------------------------------------------------------------------------
#ifndef BASETP_H
#define BASETP_H

/// Association of basic types with short names. Some definitions specify the
/// data size not dependently of the compiler.
//------------------------------------------------------------------------------
namespace baseTp {

typedef unsigned char       uchar;      ///< Unsigned char.
typedef unsigned int        uint;       ///< Unsigned default integer. Its size
                                        ///< depends of the compiler.
typedef unsigned short      ushort;     ///< Unsigned short integer.                                        
typedef unsigned long       ulong;      ///< Unsigned long integer.

typedef          char       int8;       ///< 8 bits signed integer. Char.
typedef unsigned char       uint8;      ///< 8 bits unsigned integer. Byte.
typedef          short int  int16;      ///< 16 bits signed integer.
typedef unsigned short int  uint16;     ///< 16 bits unsigned integer. Word.
typedef          long  int  int32;      ///< 32 bits signed integer.
typedef unsigned long  int  uint32;     ///< 32 bits unsigned integer. Double
                                        ///< word.

typedef          float      float32;    ///< 32 bits float point number.
typedef          double     float64;    ///< 64 bits float point number.

}; // namespace baseTp.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#endif // BASETP_H
