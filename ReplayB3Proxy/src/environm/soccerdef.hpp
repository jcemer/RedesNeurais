#ifndef hSoccerDef
#define hSoccerDef

#pragma pack(1)

namespace phi {

namespace soccer {

struct ballInfo
{
   double x, y;
};

struct robotInfo
{
   int id,team;
   double x, y, rot;

   robotInfo(int _id=0, int _team=0, double _x=0, double _y=0, double _rot=0)
   {
      id=_id;
      team=_team;
      x=_x;
      y=_y;
      rot=_rot;
   };
};

struct motorInfo
{
   int id;
   double left, right;
};

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

} // namespace phi

#pragma pack()

#endif //hSoccerDef
