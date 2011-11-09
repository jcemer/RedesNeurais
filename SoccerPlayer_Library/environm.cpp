
#include "environm.h"
#include "soccerdef.hpp"

//------------------------------------------------------------------------------
namespace environm {

using namespace geom;

//------------------------------------------------------------------------------
namespace soccer {

////////////////////////////////////////////////////////////////////////////////
////////// robotBox

// Default constructor.
//------------------------------------------------------------------------------
robotBox::robotBox( ) {

    angle = 0;
    oldAngle = 0;
    force[0] = 0;
    force[1] = 0;
    action = 0;
}
//------------------------------------------------------------------------------


////////////////////////////////////////////////////////////////////////////////
////////// environm

// Default constructor.
//------------------------------------------------------------------------------
environm::environm( ) {

    score[0] = 0;
    score[1] = 0;
    fault = 0;
    ballOut = 0;

    worldHeight = 0.0;
    worldWidth = 0.0;
    goalDeep = 0.0;
    goalLength = 0.0;
    robotRadius = 0.0;

    robotCount = 0;
    robot = NULL;
}
//------------------------------------------------------------------------------

// Destructor.
//------------------------------------------------------------------------------
environm::~environm( ){

    this->disconnect();
    this->destroyRobots();
}
//------------------------------------------------------------------------------

// Connects to PHI simulator.
//------------------------------------------------------------------------------
bool environm::connect( char *_address ) {

    // Connects.
    if ( ! sockSim.connect( sock::sock::resolveAddress( _address ),
                            12561 ) ) {
        return false;
    }

    this->setWorld();
    this->getWorld();

    return true;
}
//------------------------------------------------------------------------------

// Disconnects of PHI simulator.
//------------------------------------------------------------------------------
void environm::disconnect( ) {

    sockSim.close();
}
//------------------------------------------------------------------------------

// Creates array of robots.
//------------------------------------------------------------------------------
bool environm::createRobots( int _robotCount ) {

    this->destroyRobots();
    if ( _robotCount <= 0 ) {
        return false;
    }

    try {
        robot = new robotBox[_robotCount];
    }
    catch ( ... ) {
        robot = NULL;
    }
    if ( robot == NULL ) {
        throw std::bad_alloc();
    }
    robotCount = _robotCount;

    return true;
}
//------------------------------------------------------------------------------

// Destroys array of robots.
//------------------------------------------------------------------------------
void environm::destroyRobots( ) {

    if ( robot != NULL ) {
        delete []robot;
        robot = NULL;
    }
    robotCount = 0;
}
//------------------------------------------------------------------------------

// Gets robot count.
//------------------------------------------------------------------------------
int environm::getRobotCount( ) const {

    return robotCount;
}
//------------------------------------------------------------------------------

// Sets the environment.
//------------------------------------------------------------------------------
void environm::setEnvironm( float _worldWidth, float _worldHeight,
                            float _goalLength, float _goalDeep,
                            float _robotRadius ) {

    worldWidth = _worldWidth / 2;
    worldHeight = _worldHeight / 2;
    goalDeep = _goalDeep;
    goalLength = _goalLength / 2;
    robotRadius = _robotRadius;

    goal[0].x = -worldWidth;
    goal[0].y = 0.0;
    goal[1].x = worldWidth;
    goal[1].y = 0.0;

    this->setWorld();
}
//------------------------------------------------------------------------------

// Gets ball position, in milimeters, whose origin is the court center.
//------------------------------------------------------------------------------
point<float> environm::getBall() const {

    return ball;
}
//------------------------------------------------------------------------------

// Gets the specified robot description.
//------------------------------------------------------------------------------
robotBox environm::getRobot( int _id ) const {

    robotBox    nullRobot;

    if ( ( _id < 0 ) || ( _id >= robotCount ) ) {
        return nullRobot;
    }
    return robot[_id];
}
//------------------------------------------------------------------------------

// Gets left goal's center. Coordinates in milimeters.
//------------------------------------------------------------------------------
point<float> environm::getLeftGoal( ) const {

    return goal[0];
}
//------------------------------------------------------------------------------

// Gets right goal's center. Coordinates in milimeters.
//------------------------------------------------------------------------------
point<float> environm::getRightGoal( ) const {

    return goal[1];
}
//------------------------------------------------------------------------------

// Gets scores at the left goal.
//------------------------------------------------------------------------------
int environm::getLeftScore( ) const {

    return score[0];
}
//------------------------------------------------------------------------------

// Gets scores at the right goal.
//------------------------------------------------------------------------------
int environm::getRightScore( ) const {

    return score[1];
}
//------------------------------------------------------------------------------

// Sets ball position.
//------------------------------------------------------------------------------
void environm::setBall( point<float> _pos ) {

    ball = _pos;

    setWorld();
}
//------------------------------------------------------------------------------

// Sets the specified robot's position in milimeters and angle in radix.
//------------------------------------------------------------------------------
void environm::setRobot( int _id, point<float> _pos, float _angle ) {

    if ( ( _id < 0 ) || ( _id >= robotCount ) ) {
        return;
    }
    robot[_id].pos = _pos;
    robot[_id].angle = _angle;
    robot[_id].oldAngle = _angle;
    robot[_id].action = 0;

    this->nearestObstacles();

    this->setWorld();
}
//------------------------------------------------------------------------------

// Does action for an specified robot.
//------------------------------------------------------------------------------
bool environm::act( int _id, float _lm, float _rm ) {

    float                   speed;
    phi::soccer::motorInfo  motor;
    int                     cmd;
    int                     event;
    int                     index;
    int                     eventCode;

    if ( ( _id < 0 ) || ( _id >= robotCount ) ) {
        return false;
    }

    // Sets robot's action.
    robot[_id].force[0] = _lm;
    robot[_id].force[1] = _rm;
    robot[_id].action = 1;

    // Verifies if every robot has acted.
    for ( index = 0; index < robotCount; index++ ) {
        if ( ! robot[index].action ) {
            break;
        }
    }
    if ( index < robotCount ) {
        return false;
    }

    // Does iteration for each robot.
    for ( index = 0; index < robotCount; index++ ) {

        robot[index].oldAngle = robot[index].angle;

        // Does the local simulation.
        if ( sockSim.getConnStatus() == sock::connStatusClosed ) {

            speed = robot[index].force[1] + robot[index].force[0];

            robot[index].angle +=
                    ( robot[index].force[1] - robot[index].force[0] ) / 2 / 10;
            if ( robot[index].angle > M_PI ) {
                robot[index].angle -= 2 * M_PI;
            }
            if ( robot[index].angle < -M_PI ) {
                robot[index].angle += 2 * M_PI;
            }

            robot[index].pos.x += speed * robotRadius *
                                  cos( robot[index].angle ) / 10;
            robot[index].pos.y += speed * robotRadius *
                                  sin( robot[index].angle ) / 10;
        }

        // Uses the simulator.
        else {

            // Sends command.
            motor.id = index + 1;
            motor.left = robot[index].force[0];
            motor.right = robot[index].force[1];
            sock::sendStruct( sockSim, cmd = phi::soccer::cmds::setMotorInfo );
            sock::sendStruct( sockSim, motor );
        }
    }

    // Executes iteration step if using the simulator.
    eventCode = -1;
    if ( sockSim.getConnStatus() != sock::connStatusClosed ) {

        sock::sendStruct( sockSim, cmd = phi::soccer::cmds::iterate );

        // Gets event.
        sock::sendStruct( sockSim, cmd = phi::soccer::cmds::getEvent );
        sock::recvStruct( sockSim, event, 1000 );
        eventCode = -1;
        if ( event != phi::soccer::events::regular ) {
            if ( event == phi::soccer::events::goal1 ) {
                score[1]++;
                eventCode = 0;
            }
            else
            if ( event == phi::soccer::events::goal2 ) {
                score[0]++;
                eventCode = 1;
            }
            else
            if ( event == phi::soccer::events::fault ) {
                fault++;
                eventCode = 2;
            }
            else
            if ( event == phi::soccer::events::ballOut ) {
                ballOut++;
                eventCode = 3;
            }

            // Puts robos on random places to restart game.
            for ( index = 0; index < robotCount; index++ ) {

                // Odd robots to right side, and even robot to left.
                if ( index & 1 ) {
                    robot[index].pos.x = ( rand() / (float) RAND_MAX ) *
                                         worldWidth / 2 + worldWidth / 4;
                }
                else {
                    robot[index].pos.x = ( rand() / (float) RAND_MAX ) *
                                         -worldWidth / 2 - worldWidth / 4;
                }
                robot[index].pos.y = ( rand() / (float) RAND_MAX ) *
                                     worldHeight - worldHeight / 2;
            }

            // Resets the world.
            ball.x = 0;
            ball.y = 0;
            this->setWorld();
        }

        // Gets the current world.
        this->getWorld();
    }

    // Look for the nearest obstacle for each robot.
    this->nearestObstacles();

    // The iteration has finished. Reset the action flag for every robot.
    for ( index = 0; index < robotCount; index++ ) {
        robot[index].action = 0;
    }

    // Warn the event.
    if ( eventCode >= 0 ) {
        this->event( eventCode );
    }

    // Trigger the iteration event.
    this->onIterate();

    return true;
}
//------------------------------------------------------------------------------

// Gets the court width.
//------------------------------------------------------------------------------
float environm::getWorldWidth( ) const {

    return worldWidth * 2;
}
//------------------------------------------------------------------------------

// Gets the court height.
//------------------------------------------------------------------------------
float environm::getWorldHeight( ) const {

    return worldHeight * 2;
}
//------------------------------------------------------------------------------

// Gets deepness of the court's goals.
//------------------------------------------------------------------------------
float environm::getGoalDeep( ) const {

    return goalDeep;
}
//------------------------------------------------------------------------------

// Get goal length in milimeters.
//------------------------------------------------------------------------------
float environm::getGoalLength( ) const {

    return goalLength * 2;
}
//------------------------------------------------------------------------------

// Gets robot radius in milimeters.
//------------------------------------------------------------------------------
float environm::getRobotRadius( ) const {

    return robotRadius;
}
//------------------------------------------------------------------------------

// Calculates distance in milimeters to the ball for the specified robot.
//------------------------------------------------------------------------------
float environm::getDistance( int _id ) const {

    if ( ( _id < 0 ) || ( _id >= robotCount ) ) {
        return 0.0;
    }

    return ( ball - robot[_id].pos ).size() - robotRadius;
}
//------------------------------------------------------------------------------

// Calculates angle difference in radians to the ball for the specified robot.
//------------------------------------------------------------------------------
float environm::getBallAngle( int _id ) const {

    float alpha;

    if ( ( _id < 0 ) || ( _id >= robotCount ) ) {
        return 0.0;
    }

    alpha = ( ball - robot[_id].pos ).angle() - robot[_id].angle;
    if ( alpha > M_PI ) {
        alpha -= 2 * M_PI;
    }
    else
    if ( alpha < -M_PI ) {
        alpha += 2 * M_PI;
    }

    return alpha;
}
//------------------------------------------------------------------------------

// Calculates angle difference in radians to the desired kick's target for the
// specified robot.
//------------------------------------------------------------------------------
float environm::getTargetAngle( int _id, point<float> _pos ) const {

    float beta;

    if ( ( _id < 0 ) || ( _id >= robotCount ) ) {
        return 0.0;
    }

    beta = ( _pos - ball ).angle() - ( ball - robot[_id].pos ).angle();
    if ( beta > M_PI ) {
        beta -= 2 * M_PI;
    }
    else
    if ( beta < -M_PI ) {
        beta += 2 * M_PI;
    }

    return beta;
}
//------------------------------------------------------------------------------

// Calculates distance in milimeters to the nearest obstacle for the specified
// robot. It is the robot collision potential.
//------------------------------------------------------------------------------
float environm::getCollision( int _id ) const {

    if ( ( _id < 0 ) || ( _id >= robotCount ) ) {
        return 0.0;
    }

    return ( robot[_id].obstacle - robot[_id].pos ).size() - robotRadius;
}
//------------------------------------------------------------------------------

// Calculates angle differencee in radians to the nearest obstacle for the
// specified robot.
//------------------------------------------------------------------------------
float environm::getObstacleAngle( int _id ) const {

    float           gamma;

    if ( ( _id < 0 ) || ( _id >= robotCount ) ) {
        return 0.0;
    }

    gamma = ( robot[_id].obstacle - robot[_id].pos ).angle() - robot[_id].angle;
    if ( gamma > M_PI ) {
        gamma -= 2 * M_PI;
    }
    else
    if ( gamma < -M_PI ) {
        gamma += 2 * M_PI;
    }

    return gamma;
}
//------------------------------------------------------------------------------

// Calculates angle moment in radians for the specified robot.
//------------------------------------------------------------------------------
float environm::getSpin( int _id ) const {

    float   spin;

    if ( ( _id < 0 ) || ( _id >= robotCount ) ) {
        return 0.0;
    }

    spin = ( robot[_id].angle - robot[_id].oldAngle );
    if ( spin > M_PI ) {
        spin -= 2 * M_PI;
    }
    if ( spin < -M_PI ) {
        spin += 2 * M_PI;
    }

    return spin;
}
//------------------------------------------------------------------------------

// Event handler. Receives 0 leftGoal; 1 rightGoal; 2 fault; 3 ballOut.
//------------------------------------------------------------------------------
void environm::event( int _event ) {

    // It does not do anything. This method should be overloaded.
}
//------------------------------------------------------------------------------

// It is triggered when an interation is done.
//------------------------------------------------------------------------------
void environm::onIterate( ) {

    // It does not do anything. This method should be overloaded.
}
//------------------------------------------------------------------------------

// Looks for the nearest obstacles.
//------------------------------------------------------------------------------
void environm::nearestObstacles( ) {

    int             index;
    int             index1;
    float           collision;
    float           dist;
    point<float>    tempPoint;

    // Look for the nearest obstacle for each robot.
    for ( index = 0; index < robotCount; index++ ) {

        // Initializes with the top right corner.
        collision = worldWidth + worldHeight;
        robot[index].obstacle.x = worldWidth;
        robot[index].obstacle.y = worldHeight;

        // In front of the goals.
        if ( ( robot[index].pos.y <  goalLength ) &&
             ( robot[index].pos.y > -goalLength ) ) {
            // Right wall.
            if ( ( goalDeep + worldWidth - robot[index].pos.x ) <= collision ) {
                collision = goalDeep + worldWidth - robot[index].pos.x;
                robot[index].obstacle.x = worldWidth + goalDeep;
                robot[index].obstacle.y = robot[index].pos.y;
            }
            // Left wall.
            if ( ( robot[index].pos.x +worldWidth +goalDeep ) <= collision ) {
                collision = robot[index].pos.x +worldWidth +goalDeep;
                robot[index].obstacle.x = -worldWidth -goalDeep;
                robot[index].obstacle.y = robot[index].pos.y;
            }
            // Left goal top corner.
            tempPoint.x = -worldWidth;
            tempPoint.y = +goalLength;
            if ( ( robot[index].pos - tempPoint ).size() <= collision ) {
                collision = ( robot[index].pos - tempPoint ).size();
                robot[index].obstacle = tempPoint;
            }
            // Left goal bottom corner.
            tempPoint.x = -worldWidth;
            tempPoint.y = -goalLength;
            if ( ( robot[index].pos - tempPoint ).size() <= collision ) {
                collision = ( robot[index].pos - tempPoint ).size();
                robot[index].obstacle = tempPoint;
            }              
            // Right goal top corner.
            tempPoint.x = +worldWidth;
            tempPoint.y = +goalLength;
            if ( ( robot[index].pos - tempPoint ).size() <= collision ) {
                collision = ( robot[index].pos - tempPoint ).size();
                robot[index].obstacle = tempPoint;
            }
            // Right goal bottom corner.
            tempPoint.x = +worldWidth;
            tempPoint.y = -goalLength;
            if ( ( robot[index].pos - tempPoint ).size() <= collision ) {
                collision = ( robot[index].pos - tempPoint ).size();
                robot[index].obstacle = tempPoint;
            }
        }
        else {
            // Right wall.
            if ( ( worldWidth - robot[index].pos.x ) <= collision ) {
                collision = worldWidth - robot[index].pos.x;
                robot[index].obstacle.x = worldWidth;
                robot[index].obstacle.y = robot[index].pos.y;
            }
            // Left wall.
            if ( ( robot[index].pos.x - -worldWidth ) <= collision ) {
                collision = robot[index].pos.x - -worldWidth;
                robot[index].obstacle.x = -worldWidth;
                robot[index].obstacle.y = robot[index].pos.y;
            }
        }
        // Top wall.
        if ( ( worldHeight - robot[index].pos.y ) <= collision ) {
            collision = worldHeight - robot[index].pos.y;
            robot[index].obstacle.y = worldHeight;
            robot[index].obstacle.x = robot[index].pos.x;
        }
        // Bottom wall.
        if ( ( robot[index].pos.y - -worldHeight ) <= collision ) {
            collision = robot[index].pos.y - -worldHeight;
            robot[index].obstacle.y = -worldHeight;
            robot[index].obstacle.x = robot[index].pos.x;
        }

        // Test the other robots.
        for ( index1 = 0; index1 < robotCount; index1++ ) {
            if ( index1 == index ) {
                continue;
            }
            dist = ( robot[index].pos - robot[index1].pos ).size() -robotRadius;
            if ( dist < collision ) {
                collision = dist;
                robot[index].obstacle = robot[index1].pos;
                tempPoint = ( robot[index1].pos - robot[index].pos );
                tempPoint = tempPoint - ( ( tempPoint *
                                            ( 1.0 / tempPoint.size() ) ) *
                                          robotRadius );
                robot[index].obstacle = tempPoint + robot[index].pos;
            }
        }
    }
}
//------------------------------------------------------------------------------

// Transmits the world to the simulator.
//------------------------------------------------------------------------------
void environm::setWorld( )
{
    phi::soccer::ballInfo   ballSend;
    phi::soccer::robotInfo  robotSend;
    int                     cmd;
    int                     index;

    if ( sockSim.getConnStatus() == sock::connStatusClosed ) {
        return;
    }

    // Sends robot count.
    sock::sendStruct( sockSim, cmd = phi::soccer::cmds::setRobotCount );
    sock::sendStruct( sockSim, robotCount );

    // Sends the robots.
    for ( index = 0; index < robotCount; index++ ) {

        // Angle has to be 90.
        robot[index].angle = M_PI / 2;

        // Builds and sends robot's description.
        sock::sendStruct( sockSim, cmd = phi::soccer::cmds::setRobotInfo );
        robotSend = phi::soccer::robotInfo( index + 1, 1,
                                            robot[index].pos.x / 10.0,
                                            robot[index].pos.y / 10.0,
                                            robot[index].angle / M_PI * 180 );
        sock::sendStruct( sockSim, robotSend );
    }

    // Sends the ball.
    sock::sendStruct( sockSim, cmd = phi::soccer::cmds::setBallInfo );
    ballSend.x = ball.x / 10.0;
    ballSend.y = ball.y / 10.0;
    sock::sendStruct( sockSim, ballSend );

    // Creates the scene.
    sock::sendStruct( sockSim, cmd = phi::soccer::cmds::buildScene );
}
//------------------------------------------------------------------------------

// Gets the world information from the simulator.
//------------------------------------------------------------------------------
void environm::getWorld()
{
    phi::soccer::ballInfo   ballRecv;
    phi::soccer::robotInfo  robotRecv;
    int                     cmd;
    int                     count;
    int                     index;

    if ( sockSim.getConnStatus() == sock::connStatusClosed ) {
        return;
    }

    // Receives the ball.
    ballRecv.x = 0;
    ballRecv.y = 0;
    sock::sendStruct( sockSim, cmd = phi::soccer::cmds::getBallInfo );
    sock::recvStruct( sockSim, ballRecv, 1000 );
    ball.x = ballRecv.x * 10;
    ball.y = ballRecv.y * 10;

    // Gets robot count.
    count = 0;
    sock::sendStruct( sockSim, cmd = phi::soccer::cmds::getRobotCount );
    sock::recvStruct( sockSim, count, 1000 );
    if ( count != robotCount ) {
        return;
    }

    // Receives the robots.
    for ( index = 0; index < robotCount; index++ ) {

        // Receives robot's description.
        robotRecv.id = 0;
        robotRecv.team = 0;
        robotRecv.x = 0.0;
        robotRecv.y = 0.0;
        robotRecv.rot = 0.0;
        sock::sendStruct( sockSim, cmd = phi::soccer::cmds::getRobotInfo );
        sock::recvStruct( sockSim, robotRecv, 1000 );
        robot[index].pos.x = robotRecv.x * 10.0;
        robot[index].pos.y = robotRecv.y * 10.0;
        robot[index].angle = robotRecv.rot / 180.0 * M_PI;
    }
}
//------------------------------------------------------------------------------


////////////////////////////////////////////////////////////////////////////////
////////// clientEnvironm.

// Default constructor.
//------------------------------------------------------------------------------
clientEnvironm::clientEnvironm( ) {

    id = -1;
}
//------------------------------------------------------------------------------

// Connects to the server.
//------------------------------------------------------------------------------
bool clientEnvironm::connect( char *_address, int _port ) {

    int     command;    // Send/Receive command.

    id = -1;

    this->onSockEvent( "Connecting to the environment server" );


    // Connects to the environment server.
    if ( ! sockSim.connect( sock::sock::resolveAddress( _address ), _port ) ) {
        return false;
    }

    this->onSockEvent( "Waiting player id" );

    // Waits for the robot identification.
    while ( ! sock::recvStruct( sockSim, command, 15000 ) ) {
        if ( sockSim.getConnStatus() == sock::connStatusClosed ) {
            this->onSockEvent( "Fail receiving player id" );
            sockSim.close();
            return false;
        }
        this->onSockEvent( "Timeout waiting player id" );
    }
    if ( ( command < 0 ) || ( command >= 2 ) ) {
        this->onSockEvent( "Invalid player id" );
        sockSim.close();
        return false;
    }
    id = command;

    this->onSockEvent( "Getting world's description" );

    // Gets world's description.
    sock::sendStruct( sockSim, command = cmdGetWorld );
    while ( ! sock::recvStruct( sockSim, robotCount, 15000 ) ) {
        if ( sockSim.getConnStatus() == sock::connStatusClosed ) {
            this->onSockEvent( "Fail waiting robot count" );
            sockSim.close();
            return false;
        }
        this->onSockEvent( "Timeout waiting world's description" );
    }
    if ( ! sock::recvStruct( sockSim, robotRadius ) ) {
        this->onSockEvent( "Fail waiting robot radius" );
        sockSim.close();
        return false;
    }
    if ( ! sock::recvStruct( sockSim, worldWidth ) ) {
        this->onSockEvent( "Fail waiting world width" );
        sockSim.close();
        return false;
    }
    if ( ! sock::recvStruct( sockSim, worldHeight ) ) {
        this->onSockEvent( "Fail waiting world height" );
        sockSim.close();
        return false;
    }
    if ( ! sock::recvStruct( sockSim, goalLength ) ) {
        this->onSockEvent( "Fail waiting goal length" );
        sockSim.close();
        return false;
    }
    if ( ! sock::recvStruct( sockSim, goalDeep ) ) {
        this->onSockEvent( "Fail waiting goal deepness" );
        sockSim.close();
        return false;
    }
    goal[0].x = -worldWidth;
    goal[0].y = 0.0;
    goal[1].x = worldWidth;
    goal[1].y = 0.0;

    // Create robots.
    if ( ! this->createRobots( robotCount ) ) {
        sockSim.close();
        return false;
    }

    this->onSockEvent( "Getting match status" );

    // Gets match score and robots and ball descriptions.
    return this->getMatchStatus();
}
//------------------------------------------------------------------------------

// Disconnects of the server.
//------------------------------------------------------------------------------
void clientEnvironm::disconnect( ) {

    environm::disconnect();
}
//------------------------------------------------------------------------------

// Gets player id.
//------------------------------------------------------------------------------
int clientEnvironm::getId( ) const {

    return id;
}
//------------------------------------------------------------------------------

// Gets ball position, in milimeters, whose origin is the court center.
//------------------------------------------------------------------------------
point<float> clientEnvironm::getBall( ) const {

    return ball;
}
//------------------------------------------------------------------------------

// Gets the own robot description.
//------------------------------------------------------------------------------
robotBox clientEnvironm::getOwnRobot( ) const {

    robotBox    nullRobot;

    if ( ( id < 0 ) || ( id >= robotCount ) || ( robot == NULL ) ) {
        return nullRobot;
    }

    return robot[id];
}
//------------------------------------------------------------------------------

// Gets the rival robot description.
//------------------------------------------------------------------------------
robotBox clientEnvironm::getRivalRobot( ) const {

    robotBox    nullRobot;

    if ( ( id < 0 ) || ( id >= robotCount ) || ( robot == NULL ) ) {
        return nullRobot;
    }

    return robot[!id];
}
//------------------------------------------------------------------------------

// Gets own attacking goal's center.
//------------------------------------------------------------------------------
point<float> clientEnvironm::getOwnGoal( ) const {

    point<float>    nullPoint;

    if ( ( id < 0 ) || ( id >= robotCount ) || ( robot == NULL ) ) {
        return nullPoint;
    }

    return goal[!id];
}
//------------------------------------------------------------------------------

// Gets attacking goal's center of the rival.
//------------------------------------------------------------------------------
point<float> clientEnvironm::getRivalGoal( ) const {

    point<float>    nullPoint;

    if ( ( id < 0 ) || ( id >= robotCount ) || ( robot == NULL ) ) {
        return nullPoint;
    }

    return goal[id];
}
//------------------------------------------------------------------------------

// Gets scores of the own robot.
//------------------------------------------------------------------------------
int clientEnvironm::getOwnScore( ) const {

    if ( ( id < 0 ) || ( id >= robotCount ) || ( robot == NULL ) ) {
        return 0;
    }

    return score[!id];
}
//------------------------------------------------------------------------------

// Gets scores of the rival's robot.
//------------------------------------------------------------------------------
int clientEnvironm::getRivalScore( ) const {

    if ( ( id < 0 ) || ( id >= robotCount ) || ( robot == NULL ) ) {
        return 0;
    }

    return score[id];
}
//------------------------------------------------------------------------------

// Does action for the robot.
//------------------------------------------------------------------------------
bool clientEnvironm::act( float _lm, float _rm ) {

    int     command;

    if ( ( id < 0 ) || ( id >= robotCount ) || ( robot == NULL ) ) {
        return false;
    }
    if ( sockSim.getConnStatus() != sock::connStatusClient ) {
        return false;
    }

    // Sends command.
    sock::sendStruct( sockSim, command = cmdAct );
    sock::sendStruct( sockSim, id );
    sock::sendStruct( sockSim, _lm );
    sock::sendStruct( sockSim, _rm );

    // Gets match score and robots and ball descriptions.
    return this->getMatchStatus( false );
}
//------------------------------------------------------------------------------

// Gets the court width in milimeters.
//------------------------------------------------------------------------------
float clientEnvironm::getWorldWidth( ) const {

    return worldWidth * 2;
}
//------------------------------------------------------------------------------

// Gets the court height in milimeters.
//------------------------------------------------------------------------------
float clientEnvironm::getWorldHeight( ) const {

    return worldHeight * 2;
}
//------------------------------------------------------------------------------

// Gets deepness of the court's goals.
//------------------------------------------------------------------------------
float clientEnvironm::getGoalDeep( ) const {

    return goalDeep;
}
//------------------------------------------------------------------------------

// Get goal length in milimeters.
//------------------------------------------------------------------------------
float clientEnvironm::getGoalLength( ) const {

    return goalLength * 2;
}
//------------------------------------------------------------------------------

// Gets robot radius in milimeters.
//------------------------------------------------------------------------------
float clientEnvironm::getRobotRadius( ) const {

    return robotRadius;
}
//------------------------------------------------------------------------------

// Calculates distance in milimeters to the ball for the robot.
//------------------------------------------------------------------------------
float clientEnvironm::getDistance( ) const {

    if ( ( id < 0 ) || ( id >= robotCount ) || ( robot == NULL ) ) {
        return 0.0;
    }

    return environm::getDistance( id );
}
//------------------------------------------------------------------------------

// Calculates angle difference in radians to the ball for the robot.
//------------------------------------------------------------------------------
float clientEnvironm::getBallAngle( ) const {

    if ( ( id < 0 ) || ( id >= robotCount ) || ( robot == NULL ) ) {
        return 0.0;
    }

    return environm::getBallAngle( id );
}
//------------------------------------------------------------------------------

// Calculates angle difference in radians to the desired kick's target for
// the robot.
//------------------------------------------------------------------------------
float clientEnvironm::getTargetAngle( point<float> _pos ) const {

    if ( ( id < 0 ) || ( id >= robotCount ) || ( robot == NULL ) ) {
        return 0.0;
    }

    return environm::getTargetAngle( id, _pos );
}
//------------------------------------------------------------------------------

// Calculates distance in milimeters to the nearest obstacle for the robot.
//------------------------------------------------------------------------------
float clientEnvironm::getCollision( ) const {

    if ( ( id < 0 ) || ( id >= robotCount ) || ( robot == NULL ) ) {
        return 0.0;
    }

    return environm::getCollision( id );
}
//------------------------------------------------------------------------------

// Calculates angle differencee in radians to the nearest obstacle for the
// robot.
//------------------------------------------------------------------------------
float clientEnvironm::getObstacleAngle( ) const {

    if ( ( id < 0 ) || ( id >= robotCount ) || ( robot == NULL ) ) {
        return 0.0;
    }

    return environm::getObstacleAngle( id );
}
//------------------------------------------------------------------------------

// Calculates angle moment in radians for the robot.
//------------------------------------------------------------------------------
float clientEnvironm::getSpin( ) const {

    if ( ( id < 0 ) || ( id >= robotCount ) || ( robot == NULL ) ) {
        return 0.0;
    }

    return environm::getSpin( id );
}
//------------------------------------------------------------------------------

// Gets match status.
//------------------------------------------------------------------------------
bool clientEnvironm::getMatchStatus( bool _ask ) {

    int     command;

    if ( sockSim.getConnStatus() != sock::connStatusClient ) {
        return false;
    }

    // Asks for the match status.
    if ( _ask ) {
        sock::sendStruct( sockSim, command = cmdGetMatchStatus );
    }

    // Receives the ball.
    while ( ! sock::recvStruct( sockSim, ball, 15000 ) ) {
        if ( sockSim.getConnStatus() == sock::connStatusClosed ) {
            this->onSockEvent( "Fail receiving ball description" );
            sockSim.close();
            return false;
        }
        this->onSockEvent( "Timeout waiting match status" );
    }

    // Receives robot count.
    if ( ! sock::recvStruct( sockSim, command ) ) {
        this->onSockEvent( "Fail receiving robot count" );
        sockSim.close();
        return false;
    }
    if ( command != robotCount ) {
        this->onSockEvent( "Invalid robot count" );
        sockSim.close();
        return false;
    }

    // Receives robots.
    for ( int i = 0; i < robotCount; i++ ) {
        if ( ! sock::recvStruct( sockSim, robot[i] ) ) {
            this->onSockEvent( "Fail receiving robot description" );
            sockSim.close();
            return false;
        }
    }

    // Receives score.
    if ( ! sock::recvStruct( sockSim, score[0] ) ) {
        this->onSockEvent( "Fail receiving scores" );
        sockSim.close();
        return false;
    }
    if ( ! sock::recvStruct( sockSim, score[1] ) ) {
        this->onSockEvent( "Fail receiving scores" );
        sockSim.close();
        return false;
    }

    return true;
}
//------------------------------------------------------------------------------

// Triggered for socket events.
//------------------------------------------------------------------------------
void clientEnvironm::onSockEvent( char *_msg ) {

    // It does not do anything. It could be overloaded.
}
//------------------------------------------------------------------------------

}; // namespace soccer.
//------------------------------------------------------------------------------

}; // namespace environm.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
