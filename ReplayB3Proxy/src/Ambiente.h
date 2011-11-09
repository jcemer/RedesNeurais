#ifndef AMBIENTE_H
#define AMBIENTE_H

#include "environm/environm.h"
#include <fstream>

#define FUZZY_PI 3.14159265

#define ANGULO              0
#define DIRECAO_ALVO        1
#define DISTANCE_COLLISION  2
#define DISTANCE_BALL       3
#define VARIABLE_LEN        4

class Ambiente
{
  public:
    Ambiente(char *_address, int _portClient, int _portServer);//Todo: connect parameters ??
    virtual ~Ambiente();//Todo: disconnect stuff ??
    void reconnect(char* address, int portClient, int portServer);

    void readInputs();
    void readFromLogFile(std::ifstream& logFile, double& leftWheel, double& rightWheel);
    void setRobotPositionToLogPosition();
    void sendOutput();
    void sendOutput(double leftWheel, double rightWheel);

    void outputLogHeader(std::ostream& os);
    void outputLogData(std::ostream& os);

    double variables[VARIABLE_LEN];
    double evaluation;
    double output;
    float  logDist, logBallAng, logTarAng, logColl, logObstAng, logSpin, logFor0, logFor1;
    float  ballAng, goalAng, dist, goalDeep, goalOwnAng, goalRivAng, obstAng, coll, spin;
    geom::point<float> position;
    geom::point<float> logPosition;

    int oldGoalsLeft, oldGoalsRight, goalsLeft, goalsRight;

  private:
    environm::soccer::clientEnvironm environment;
};


#endif // AMBIENTE_H
