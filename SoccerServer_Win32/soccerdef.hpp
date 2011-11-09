#ifndef hSoccerDef
#define hSoccerDef

namespace soccer {

#pragma pack(1)

struct ballInfo
{
   double x, y;
};

struct robotInfo
{
   int id,team;
   double x, y, rot;
};

struct motorInfo
{
   int id;
   double left, right;
};

#pragma pack()

namespace cmds {
   enum command 
   {
      buildScene,
      iterate,
      getRobotCount,
      setRobotCount,
      getBallInfo,
      setBallInfo,
      getRobotInfo,
      setRobotInfo,
      getMotorInfo,
      setMotorInfo,
      getEvent,
      startLog,
      stopLog
   };
};

namespace events {
   enum event
   {
      regular,
      goal1,
      goal2,
      ballOut,
      fault
   };
};

} // namespace soccer

#endif //hSoccerDef
