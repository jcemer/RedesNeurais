#include "Ambiente.h"

#include <stdlib.h>
#include <math.h>

#include<iostream>
using namespace std;

//////////////////////////////////////////////////
Ambiente::Ambiente(char* address, int portClient, int portServer)
{
  if( this->environment.connect(address, portClient, portServer) == false)
  {
    exit(-1);
  }
}
//////////////////////////////////////////////////
Ambiente::~Ambiente()
{
  this->environment.disconnect();
}
//////////////////////////////////////////////////
void Ambiente::reconnect(char* address, int portClient, int portServer)
{
  if( this->environment.connect(address, portClient, portServer) == false)
  {
    exit(-1);
  }
}
//////////////////////////////////////////////////
void Ambiente::readFromLogFile(ifstream& logFile, double& leftWheel, double& rightWheel)
{
  string line;

  //read an un-commented line
  getline(logFile, line);
  while(line.find("//") != string::npos) getline(logFile, line);

  //cout << line << endl;

  //parse it
  sscanf(line.c_str(), "%f,%f,%f,%f,%f,%f,%f,%f",
              &this->logDist,
              &this->logBallAng,
              &this->logTarAng,
              &this->logColl,
              &this->logObstAng,
              &this->logSpin,
              &this->logFor0,
              &this->logFor1);

  this->logPosition = geom::point<float>
                      (
                      this->logDist * cos(this->logBallAng),
                      this->logDist * sin(this->logBallAng)
                      );

  cout << "this->logDist: " << this->logDist << endl;
  cout << "this->logBallAng: " << this->logBallAng << endl;
  cout << "this->logTarAng: " << this->logTarAng << endl;
  cout << "this->logColl: " << this->logColl << endl;
  cout << "this->logObstAng: " << this->logObstAng << endl;
  cout << "this->logSpin: " << this->logSpin << endl;
  cout << "this->logFor0: " << this->logFor0 << endl;
  cout << "this->logFor1: " << this->logFor1 << endl;
  cout << "-------------------------------------------------------" << endl;

  //output our forces
  leftWheel = this->logFor0;
  rightWheel = this->logFor1;

  //done
  return;
}
//////////////////////////////////////////////////
void Ambiente::readInputs()
{
//  for (int i = 0; i < VARIABLE_LEN; i++)
//  {
//    switch(i)
//    {
//      case ANGULO:              this->variables[i] = this->environment.getBallAngle(); break;
//      //case DIRECAO_ALVO:  this->variables[i] = this->environment.getTargetAngle(this->environment.getOwnGoal()); break;
//      case DIRECAO_ALVO:  this->variables[i] = this->environment.getTargetAngle(this->environment.getRivalGoal()); break;
//      //case DIRECAO_ALVO:        this->variables[i] = this->environment.getGoalAngle(); break;
//      case DISTANCE_COLLISION:  this->variables[i] = this->environment.getCollision(); break;
//      case DISTANCE_BALL:       this->variables[i] = this->environment.getDistance(); break;
//    }
//  }
  this->ballAng = this->environment.getBallAngle();
  this->goalAng = this->environment.getGoalAngle();
  this->dist    = this->environment.getDistance();
  this->goalOwnAng = this->environment.getTargetAngle(this->environment.getOwnGoal());
  this->goalRivAng = this->environment.getTargetAngle(this->environment.getRivalGoal());
  this->obstAng = this->environment.getObstacleAngle();
  this->coll = this->environment.getCollision();
  this->spin = this->environment.getSpin();
  this->position = this->environment.getOwnRobot().pos;

  geom::point<float> point = geom::point<float>
                      (
                      this->dist * sin(this->ballAng - this->goalOwnAng),
                      this->dist * cos(this->ballAng - this->goalOwnAng)
                      );

  if (this->position.x != point.x)
    cout << "position.x != calculatedPoint.x ==> " <<
        this->position.x <<
        " != " <<
        point.x <<
        endl;

  if (this->position.y != point.y)
    cout << "position.y != calculatedPoint.y ==> " <<
        this->position.y <<
        " != " <<
        point.y <<
        endl;

  this->oldGoalsLeft = this->goalsLeft;
  this->oldGoalsRight = this->goalsRight;
  this->goalsLeft = this->environment.getOwnScore();
  this->goalsRight = this->environment.getRivalScore();

  cout << "getBallAngle(): " << this->ballAng << endl;
  cout << "getGoalAngle(): " << this->goalAng << endl;
  cout << "getDistance(): " << this->dist << endl;
  cout << "getTagetAngle(own goal): " << this->goalOwnAng << endl;
  cout << "getTagetAngle(rival goal): " << this->goalRivAng << endl;
  cout << "getObstacleAngle(): " << this->obstAng << endl;
  cout << "getCollision(): " << this->coll << endl;
  cout << "getSpin(): " << this->spin << endl;
  cout << "=======================================================" << endl;

}
//////////////////////////////////////////////////
void Ambiente::setRobotPositionToLogPosition()
{
  this->environment.setRobot(this->environment.getId(),
                             this->logPosition,
                             0.0);
}
//////////////////////////////////////////////////
void Ambiente::sendOutput()
{
  float leftMotor  = cos( this->output*FUZZY_PI/180 ) - sin( this->output*FUZZY_PI/180 );
  float rightMotor = cos( this->output*FUZZY_PI/180 ) + sin( this->output*FUZZY_PI/180 );

  this->sendOutput(leftMotor, rightMotor);
}
//////////////////////////////////////////////////
void Ambiente::sendOutput(double leftWheel, double rightWheel)
{
  bool ok;

  //sanity - to avoid being unstable
  //double leftWheel = min(0.7, _leftWheel);
  //double rightWheel = min(0.7, _rightWheel);

  //to handle inercy (inercia ?), send some times...
  ok = this->environment.act( leftWheel, rightWheel ); if (!ok) goto Error;
  //ok = this->environment.act( leftWheel, rightWheel ); if (!ok) goto Error;
  //ok = this->environment.act( leftWheel, rightWheel ); if (!ok) goto Error;
  //ok = this->environment.act( leftWheel, rightWheel ); if (!ok) goto Error;
  //ok = this->environment.act( leftWheel, rightWheel ); if (!ok) goto Error;
  //ok = this->environment.act( leftWheel, rightWheel ); if (!ok) goto Error;

  //and then reverse the engines
  //ok = this->environment.act( -leftWheel, -rightWheel ); if (!ok) goto Error;
  //ok = this->environment.act( -leftWheel, -rightWheel ); if (!ok) goto Error;
  //ok = this->environment.act( -leftWheel, -rightWheel ); if (!ok) goto Error;

  //and stop it all
  //ok = this->environment.act( 0, 0 ); if (!ok) goto Error;

Error:
  //and threat some error...
  if (!ok)
  {
    exit(-1);
  }
}
//////////////////////////////////////////////////
void Ambiente::outputLogHeader(std::ostream& os)
{
  os << "//x y oldx oldy angle oldangle ballAng ballX ballY goalAng dist obstAng obstX obstY collAng spin for0 for1" << "\n";
}
//////////////////////////////////////////////////
void Ambiente::outputLogData(std::ostream& os)
{
  os.setf(std::ios::fixed,std::ios::floatfield);            // floatfield not set
  os.precision(3);

  os << " " << this->environment.getOwnRobot().pos.x;
  os << ", " << this->environment.getOwnRobot().pos.y;
  os << ", " << this->environment.getOwnRobot().oldPos.x;
  os << ", " << this->environment.getOwnRobot().oldPos.y;
  os << ", " << this->environment.getOwnRobot().angle;
  os << ", " << this->environment.getOwnRobot().oldAngle;
  os << ", " << this->environment.getBallAngle();
  os << ", " << this->environment.getBall().x;
  os << ", " << this->environment.getBall().y;
  os << ", " << this->environment.getGoalAngle();
  os << ", " << this->environment.getDistance();
  os << ", " << this->environment.getObstacleAngle();
  os << ", " << this->environment.getOwnRobot().obstacle.x;
  os << ", " << this->environment.getOwnRobot().obstacle.y;
  os << ", " << this->environment.getCollision();
  os << ", " << this->environment.getSpin();
  os << ", " << this->environment.getOwnRobot().force[0];
  os << ", " << this->environment.getOwnRobot().force[1];
  os << "\n";
}
//////////////////////////////////////////////////
