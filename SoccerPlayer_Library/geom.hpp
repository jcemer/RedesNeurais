// Developed by Eduardo Wisnieski Basso - http://www.inf.ufrgs.br/~ewbasso 
// NO WARRANTY is given over this source code. You are free to use or change
// this source code as you wish. Contact-me through mailto:ewbasso@inf.ufrgs.br.
//------------------------------------------------------------------------------
#ifndef GEOM_HPP
#define GEOM_HPP

#include <math.h>

/// Template classes describing geometry objects.
//------------------------------------------------------------------------------
namespace geom {

// Previous declaration of rect.
template <class type> class rect;

/// A (2D) point in a plane.
//------------------------------------------------------------------------------
template <class type>
class point {
public:
    type    x;  ///< X coordinate of the point in the plane.
    type    y;  ///< Y coordinate of the point in the plane.

    /// Initializing constructor.
    /// @param _x: Coordinate x of the point. Default is 0.
    /// @param _y: Coordinate y of the point. Default is 0.
    point( type _x=0, type _y=0 );

    /// Copy constructor.
    /// @param _point: Point to be copied.
    point( const point<type> &_point );

    /// Negates the signals of coordinates.
    /// @return Negated point.
    point<type> operator-( ) const;

    /// Gets the distance from origin until the point.
    /// @return Euclidian distance from origin.
    type size( ) const;

    /// Calculates the angle for the vector from the origin to this point.
    /// @return Angle in radians, where x-axis is angle 0.
    /// @return Zero if the vector is null (point at origin).
    float angle( ) const;

    /// Moves the point with an offset vector. Sum of points.
    /// @param _vector: Offset vector to be added.
    /// @return The point moved by the vector.
    point<type> operator+( const point<type> &_vector ) const;

    /// Gets the distance vector from another point. Subtraction of points.
    /// @param _point: Origin point of the vector.
    /// @return Distance vector.
    point<type> operator-( const point<type> &_point ) const;

    /// Applies an scalar coefficient over coordinates.
    /// @param _coef: Scalar coefficient to be applied.
    /// @return The original point times the scalar coefficient.
    point<type> operator*( const type &_coef ) const;

    /// Does the scalar product with other vector.
    /// @param _vector: Second vector in the product.
    /// @return The original point times the scalar coefficient.
    type operator*( const point<type> &_vector ) const;

    /// Verifies if the point is in a rectangle.
    /// @param _rect: Rectangle where the point must be in.
    /// @return True if the point is in the rectangle, or false if it is not.
    bool test( const rect<type> &_rect ) const;

    /// Makes a point to occur in a rectangle. It truncates the coordinates
    /// those lie out the rectangle. The current point is not changed.
    /// @param _rect: Rectangle where the point must be in.
    /// @return Point moved into the region of interest.
    point<type> fit( const rect<type> &_rect ) const;

    /// Rotates the point as a vector from origin. Positive angle turns the
    /// vector to left and negative angle turns the vector to right.
    /// @param _cosAng: Cosine of the rotating angle.
    /// @param _sinAng: Sine of the rotating angle.
    /// @return Ending point of the rotated vector.
    point<type> rotate( float _cosAng, float _sinAng ) const;
};
//------------------------------------------------------------------------------

/// A (2D) line in a plane.
//------------------------------------------------------------------------------
template <class type>
class line {
public:
    point<type> point0;     ///< Starting point of line.
    point<type> point1;     ///< Ending point of line.

    /// Default constructor.
    line( );

    /// Initializing constructor.
    /// @param _point0: Starting point of line.
    /// @param _point1: Ending point of line.
    line( const point<type> &_point0, const point<type> &_point1 );

    /// Copy constructor.
    /// @param _line: Line to be copied.
    line( const line<type> &_line );

    /// Moves line's points with an offset vector.
    /// @param _vector: Offset vector to be added.
    /// @return The line moved by the vector.
    line<type> operator+( const point<type> &_vector ) const;

    /// Moves line's points with a negative offset vector.
    /// @param _vector: Offset vector to be subtracted.
    /// @return The line moved by the negative vector.
    line<type> operator-( const point<type> &_vector ) const;

    /// Verifies if the line is in a rectangle.
    /// @param _rect: Rectangle where the line must be in.
    /// @return True if the line is in the rectangle, or false if it is not.
    bool test( const rect<type> &_rect ) const;

    /// Makes the line occur in a rectangle. It truncates the coordinates of
    /// points that lie out the rectangle.
    /// @param _rect: Rectangle where the line must be in.
    /// @return Line which points were moved into the region of interest.
    line<type> fit( const rect<type> &_rect ) const;
};
//------------------------------------------------------------------------------

/// A (2D) rectangle in a plane.
//------------------------------------------------------------------------------
template <class type>
class rect {
public:
    type    left;       ///< Left limit of the rectangle.
    type    bottom;     ///< Bottom limit of the rectangle.
    type    right;      ///< Right limit of the rectangle.
    type    top;        ///< Top limit of the rectangle.

    /// Initializing constructor.
    /// @param _left: Rectangle's left limit . Default is 0.
    /// @param _bottom: Rectangle's bottom limit. Default is 0.
    /// @param _right: Rectangle's right limit. Default is 0.
    /// @param _top: Rectangle's top limit. Default is 0.
    rect( type _left=0, type _bottom=0, type _right=0, type _top=0 );

    /// Copy constructor.
    /// @param _rect: Rectangle to be copied.
    rect( const rect<type> &_rect );

    /// Moves the rectangle's limits with an offset vector.
    /// @param _vector: Offset vector to be added.
    /// @return The rectangle moved by the vector.
    rect<type> operator+( const point<type> &_vector ) const;

    /// Verifies if the rectangle is valid (top >= bottom and right >= left).
    /// @return True if it is valid, or false if it is not.
    bool test( ) const;

    /// Verifies if this rectangle is in another rectangle.
    /// @param _rect: Other rectangle where this rectangle must be in.
    /// @return True if the rectangle is inside, or false if it is not.
    bool test( const rect<type> &_rect ) const;

    /// Makes this rectangle fit in another rectangle. It truncates the limits
    /// those lie out the this rectangle.
    /// @param _rect: Other rectangle where this rectangle must be in.
    /// @return Rectangle fitted in the region of interest.
    rect<type> fit( const rect<type> &_rect ) const;

    /// Verifies if a rectangle matches this rectangle. Matching means they need
    /// to have the same width and the same height.
    /// @param _rect: Rectangle to be matched.
    /// @return True if they have the same height and the same width, or false 
    /// otherwise.
    bool match( const rect<type> &_rect ) const;
};
//------------------------------------------------------------------------------



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
////////// point

/// Initializing constructor.
//------------------------------------------------------------------------------
template <class type>
point<type>::point( type _x, type _y ) {

    x = _x;
    y = _y;
}
//------------------------------------------------------------------------------

/// Copy constructor.
//------------------------------------------------------------------------------
template <class type>
point<type>::point( const point<type> &_point ) {

    x = _point.x;
    y = _point.y;
}
//------------------------------------------------------------------------------

/// Negates the signals of coordinates.
//------------------------------------------------------------------------------
template <class type>
point<type> point<type>::operator-( ) const {

    point<type>     answer;

    answer.x = - x;
    answer.y = - y;

    return answer;
}
//------------------------------------------------------------------------------

/// Gets the distance from origin until the point.
//------------------------------------------------------------------------------
template <class type>
type point<type>::size() const {

    return sqrt( x * x + y * y );
}
//------------------------------------------------------------------------------

/// Calculates the angle for the vector from the origin to this point.
//------------------------------------------------------------------------------
template <class type>
float point<type>::angle( ) const {

    if ( ( x == 0.0 ) && ( y == 0.0 ) ) {
        return 0.0;
    }
    return atan2( y, x );
}
//------------------------------------------------------------------------------

/// Moves the point with an offset vector. Sum of points.
//------------------------------------------------------------------------------
template <class type>
point<type> point<type>::operator+( const point<type> &_vector ) const {

    point<type>     answer;

    answer.x = x + _vector.x;
    answer.y = y + _vector.y;

    return answer;
}
//------------------------------------------------------------------------------

/// Gets the distance vector from another point. Subtraction of points.
//------------------------------------------------------------------------------
template <class type>
point<type> point<type>::operator-( const point<type> &_point ) const {

    point<type>     answer;

    answer.x = x - _point.x;
    answer.y = y - _point.y;

    return answer;
}
//------------------------------------------------------------------------------

/// Applies an scalar coefficient over coordinates.
//------------------------------------------------------------------------------
template <class type>
point<type> point<type>::operator*( const type &_coef ) const {

    point<type>     answer;

    answer.x = x * _coef;
    answer.y = y * _coef;

    return answer;
}
//------------------------------------------------------------------------------

/// Does the scalar product with other vector.
//------------------------------------------------------------------------------
template <class type>
type point<type>::operator*( const point<type> &_vector ) const {

    return x * _vector.x + y * _vector.y;
}
//------------------------------------------------------------------------------

/// Verifies if the point is in a rectangle.
//------------------------------------------------------------------------------
template <class type>
bool point<type>::test( const rect<type> &_rect ) const{

    if ( ( x < _rect.left ) || ( x > _rect.right ) ) {
        return false;
    }
    if ( ( y < _rect.bottom ) || ( y > _rect.top ) ) {
        return false;
    }
    return true;
}
//------------------------------------------------------------------------------

/// Makes a point to occur in a rectangle.
//------------------------------------------------------------------------------
template <class type>
point<type> point<type>::fit( const rect<type> &_rect ) const {

    point<type> answer;

    answer.x = ( x > _rect.right ) ? _rect.right :
                                    ( x < _rect.left ) ? _rect.left : x;
    answer.y = ( y > _rect.top ) ? _rect.top :
                                  ( y < _rect.bottom ) ? _rect.bottom : y;

    return answer;
}
//------------------------------------------------------------------------------

/// Rotates the point as a vector from origin.
//------------------------------------------------------------------------------
template <class type>
point<type> point<type>::rotate( float _cosAng, float _sinAng ) const {

    point<type>     answer;

    // Gets rotated vector.
    //answer.x = x * _cosAng + y * cosAng90; // cosAng90 = -_sinAng;
    //answer.y = x * _sinAng + y * sinAng90; // sinAng90 = _cosAng;
    answer.x = x * _cosAng - y * _sinAng;
    answer.y = x * _sinAng + y * _cosAng;

    return answer;
}
//------------------------------------------------------------------------------



////////////////////////////////////////////////////////////////////////////////
////////// line

/// Default constructor.
//------------------------------------------------------------------------------
template <class type>
line<type>::line( ) {

    point<type>     nullPoint;

    point0 = nullPoint;
    point1 = nullPoint;
}
//------------------------------------------------------------------------------

/// Initializing constructor.
//------------------------------------------------------------------------------
template <class type>
line<type>::line( const point<type> &_point0, const point<type> &_point1 ) {

    point0 = _point0;
    point1 = _point1;
}
//------------------------------------------------------------------------------

/// Copy constructor.
//------------------------------------------------------------------------------
template <class type>
line<type>::line( const line<type> &_line ) {

    point0 = _line.point0;
    point1 = _line.point1;
}
//------------------------------------------------------------------------------

/// Moves line's points with an offset vector.
//------------------------------------------------------------------------------
template <class type>
line<type> line<type>::operator+( const point<type> &_vector ) const {

    line<type>  answer;

    answer.point0 = point0 + _vector;
    answer.point1 = point1 + _vector;

    return answer;
}
//------------------------------------------------------------------------------

/// Moves line's points with a negative offset vector.
//------------------------------------------------------------------------------
template <class type>
line<type> line<type>::operator-( const point<type> &_vector ) const {

    line<type>  answer;

    answer.point0 = point0 - _vector;
    answer.point1 = point1 - _vector;

    return answer;
}
//------------------------------------------------------------------------------

/// Verifies if the line is in a rectangle.
//------------------------------------------------------------------------------
template <class type>
bool line<type>::test( const rect<type> &_rect ) const {

    return ( point0.test( _rect ) && point1.test( _rect ) );
}
//------------------------------------------------------------------------------

/// Makes the line occur in a rectangle.
//------------------------------------------------------------------------------
template <class type>
line<type> line<type>::fit( const rect<type> &_rect ) const {

    line<type>  answer;

    answer.point0 = point0.fit( _rect );
    answer.point1 = point1.fit( _rect );

    return answer;
}
//------------------------------------------------------------------------------


////////////////////////////////////////////////////////////////////////////////
////////// rect

/// Initializing constructor.
//------------------------------------------------------------------------------
template <class type>
rect<type>::rect( type _left, type _bottom, type _right, type _top ) {

    left = _left;
    bottom = _bottom;
    right = _right;
    top = _top;
}
//------------------------------------------------------------------------------

/// Copy constructor.
//------------------------------------------------------------------------------
template <class type>
rect<type>::rect( const rect<type> &_rect ) {

    left = _rect.left;
    bottom = _rect.bottom;
    right = _rect.right;
    top = _rect.top;
}
//------------------------------------------------------------------------------

/// Moves the rectangle's limits with an offset vector.
//------------------------------------------------------------------------------
template <class type>
rect<type> rect<type>::operator+( const point<type> &_vector ) const {

    rect<type>  answer;

    answer.left = left + _vector.x;
    answer.right = right + _vector.x;
    answer.bottom = bottom + _vector.y;
    answer.top = top + _vector.y;

    return answer;
}
//------------------------------------------------------------------------------

/// Verifies if the rectangle is valid.
//------------------------------------------------------------------------------
template <class type>
bool rect<type>::test( ) const {

    if ( left > right ) {
        return false;
    }
    if ( bottom > top ) {
        return false;
    }
    return true;
}
//------------------------------------------------------------------------------

/// Verifies if this rectangle is in another rectangle.
//------------------------------------------------------------------------------
template <class type>
bool rect<type>::test( const rect<type> &_rect ) const {

    if ( ( left < _rect.left ) || ( left > _rect.right ) ) {
        return false;
    }
    if ( ( right < _rect.left ) || ( right > _rect.right ) ) {
        return false;
    }
    if ( ( bottom < _rect.bottom ) || ( bottom > _rect.top ) ) {
        return false;
    }
    if ( ( top < _rect.bottom ) || ( top > _rect.top ) ) {
        return false;
    }
    return true;
}
//------------------------------------------------------------------------------

/// Makes this rectangle fit in another rectangle.
//------------------------------------------------------------------------------
template <class type>
rect<type> rect<type>::fit( const rect<type> &_rect ) const {

    rect<type>  answer;

    answer.left = ( left > _rect.right ) ? _rect.right :
                        ( left < _rect.left ) ? _rect.left : left;
    answer.bottom = ( bottom > _rect.top ) ? _rect.top :
                        ( bottom < _rect.bottom ) ? _rect.bottom : bottom;
    answer.right = ( right > _rect.right ) ? _rect.right :
                        ( right < _rect.left ) ? _rect.left : right;
    answer.top = ( top > _rect.top ) ? _rect.top :
                        ( top < _rect.bottom ) ? _rect.bottom : top;

    return answer;
}
//------------------------------------------------------------------------------

/// Verifies if a rectagle matches this rectangle.
//------------------------------------------------------------------------------
template <class type>
bool rect<type>::match( const rect<type> &_rect ) const {

    return ( ( _rect.right - _rect.left ) == ( right - left ) ) &&
           ( ( _rect.top - _rect.bottom ) == ( top - bottom ) );
}
//------------------------------------------------------------------------------

}; // namespace geom.
//------------------------------------------------------------------------------

#endif // GEOM_HPP
