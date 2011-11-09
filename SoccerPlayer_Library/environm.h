//------------------------------------------------------------------------------
#ifndef environmH
#define environmH

#include "sock.hpp"
#include "geom.hpp"

// Classes to simulate a mobile-robot environment.
//------------------------------------------------------------------------------
namespace environm {

using namespace geom;

// Environment classes related to robot soccer.
//------------------------------------------------------------------------------
namespace soccer {

// Commands for client-server simulation.
//------------------------------------------------------------------------------
enum cmd { cmdGetWorld, cmdGetBall, cmdGetRobot, cmdGetScore, cmdAct, cmdAck,
           cmdGetMatchStatus };
//------------------------------------------------------------------------------

// Description of a soccer player robot. The robot is a box with 2 wheels that
// can go forward (force>0) or backward (force<0).
//------------------------------------------------------------------------------
class robotBox {
public:
    point<float>    pos;        // Robot's position on the court. Coordinates in
                                // milimeters. Court's center is (0,0).
    float           angle;      // Robot's orientation [-PI..+PI].
    point<float>    oldPos;     // Last robot's positon on the court.
    float           oldAngle;   // Last iteration robot's orientation.
    point<float>    obstacle;   // Nearest obstacle for the robot.
    float           force[2];   // Left (0) and right (1) motor forces.
    bool            action;     // Says if the robot acted at this iteration.

    // Default constructor.
    robotBox( );
};
//------------------------------------------------------------------------------

// Environment of a robot soccer match. Implements the interface to a remote
// robot soccer simulator.
//------------------------------------------------------------------------------
class environm {
public:
    // Default constructor.
    environm( );

    // Destructor. It forces destruction, releasing memory and closing sockets.
    ~environm( );

    // Connects to PHI simulator.
    bool connect( char *_address );

    // Disconnects of the simulator.
    void disconnect( );

    // Creates an array of robots.
    bool createRobots( int _robotCount );

    // Destroys array of robots.
    void destroyRobots( );

    // Gets robot count.
    int getRobotCount( ) const;

    // Sets the environment. Parameters are world, goal and robot size in
    // milimeters.
    void setEnvironm( float _worldWidth, float _worldHeight,
                      float _goalLength, float _goalDeep, float _robotRadius );

    // Gets ball position, in milimeters, whose origin is the court center.
    point<float> getBall() const;

    // Gets the specified robot description.
    robotBox getRobot( int _id ) const;

    // Gets left goal's center. Coordinates in milimeters.
    point<float> getLeftGoal( ) const;

    // Gets right goal's center. Coordinates in milimeters.
    point<float> getRightGoal( ) const;

    // Gets scores at the left goal.
    int getLeftScore( ) const;

    // Gets scores at the right goal.
    int getRightScore( ) const ;

    // Sets ball position, in milimeters, whose origin is the court center.
    void setBall( point<float> _pos );

    // Sets the specified robot's position in milimeters and angle in radix.
    void setRobot( int _id, point<float> _pos, float _angle );

    // Does action for an specified robot. Receives left and right forces and
    // does an iteration if every robot has already acted.
    bool act( int _id, float _lm, float _rm );

    // Gets the court width.
    float getWorldWidth( ) const;

    // Gets the court height.
    float getWorldHeight( ) const;

    // Gets deepness of the court's goals.
    float getGoalDeep( ) const;

    // Get goal length in milimeters.
    float getGoalLength( ) const;

    // Gets robot radius in milimeters.
    float getRobotRadius( ) const;

    // Calculates distance in milimeters to the ball for the specified robot.
    float getDistance( int _id ) const;

    // Calculates angle difference in radians to the ball for the specified
    // robot.
    float getBallAngle( int _id ) const;

    // Calculates angle difference in radians to the desired kick's target for
    // the specified robot.
    float getTargetAngle( int _id, point<float> _pos ) const;

    // Calculates distance in milimeters to the nearest obstacle for the
    // specified robot. It is the robot collision potential.
    float getCollision( int _id ) const;

    // Calculates angle differencee in radians to the nearest obstacle for the
    // specified robot.
    float getObstacleAngle( int _id ) const;

    // Calculates angle moment in radians for the specified robot.
    float getSpin( int _id ) const;

protected:
    float           worldWidth; // Half of the world width.
    float           worldHeight;// Half of the world height.
    float           goalDeep;   // Deepness of the field's goals.
    float           goalLength; // Half of the goal length.
    float           robotRadius;// Robot radius.

    sock::sock      sockSim;    // Socket to connect to the simulator.
    int             robotCount; // Number of robots.

    point<float>    ball;       // Ball position on the court.
    robotBox       *robot;      // Robots on the court.

    point<float>    goal[2];    // Left (0) and right (1) goal's center.

    int             score[2];   // Scores at the left (0) and right (1) goals.
    int             fault;      // Fault count.
    int             ballOut;    // Ball out count;

    // Event handler. Receives 0 leftGoal; 1 rightGoal; 2 fault; 3 ballOut.
    virtual void event( int _event );

    // It is triggered when an interation is done.
    virtual void onIterate( );

    // Looks for the nearest obstacles.
    void nearestObstacles( );

    // Transmits the world to the simulator.
    void setWorld( );

    // Gets the world information from the simulator.
    void getWorld( );
};
//------------------------------------------------------------------------------

// Client interface for a robot soccer match simulator.
//------------------------------------------------------------------------------
class clientEnvironm : protected environm {

public:
    // Default constructor.
    clientEnvironm( );

    // Connects to the server. The port identifies the robot.
    bool connect( char *_address, int _port );

    // Disconnects of the server.
    void disconnect( );

    // Gets player id.
    int getId( ) const;

    // Gets ball position, in milimeters, whose origin is the court center.
    point<float> getBall( ) const;

    // Gets the own robot description.
    robotBox getOwnRobot( ) const;

    // Gets the rival robot description.
    robotBox getRivalRobot( ) const;

    // Gets own attacking goal's center.
    point<float> getOwnGoal( ) const;

    // Gets attacking goal's center of the rival.
    point<float> getRivalGoal( ) const;

    // Gets scores of the own robot.
    int getOwnScore( ) const;

    // Gets scores of the rival robot.
    int getRivalScore( ) const ;

    // Does action for the robot. Receives left and right forces. It is blocked
    // until every robot has acted.
    bool act( float _lm, float _rm );

    // Gets the court width in milimeters.
    float getWorldWidth( ) const;

    // Gets the court height in milimeters.
    float getWorldHeight( ) const;

    // Gets deepness of the court's goals.
    float getGoalDeep( ) const;

    // Get goal length in milimeters.
    float getGoalLength( ) const;

    // Gets robot radius in milimeters.
    float getRobotRadius( ) const;

    // Calculates distance in milimeters to the ball for the robot.
    float getDistance( ) const;

    // Calculates angle difference in radians to the ball for the robot.
    float getBallAngle( ) const;

    // Calculates angle difference in radians to the desired kick's target for
    // the robot.
    float getTargetAngle( point<float> _pos ) const;

    // Calculates distance in milimeters to the nearest obstacle for the robot.
    // It is the robot collision potential.
    float getCollision( ) const;

    // Calculates angle differencee in radians to the nearest obstacle for the
    // robot.
    float getObstacleAngle( ) const;

    // Calculates angle moment in radians for the robot.
    float getSpin( ) const;

protected:
    int     id;     // Robot id.

    // Gets match status.
    bool getMatchStatus( bool _ask = true );

    // Triggered for socket events.
    virtual void onSockEvent( char *_msg );
};
//------------------------------------------------------------------------------

}; // namespace soccer.
//------------------------------------------------------------------------------

}; // namespace environm.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#endif
