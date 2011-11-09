team1Class=1;
team2Class=2;
goal1Class=3;
goal2Class=4;
ballClass=5;
groundClass=7;

fieldSide=
{
   objects=
   {
      left=
      {
         prim=box;
         pos={0, 66.25, 2.5};
         size={155, 2.5, 5.0}; -- 2.5+150+2.5
      };
      leftBack=
      {
         prim=box;
         pos={76.25, 43.75, 2.5};
         size={2.5, 47.5, 5.0};
      };
      rightBack=
      {
         prim=box;
         pos={76.25, -43.75, 2.5};
         size={2.5, 47.5, 5.0};
      };
      leftBackCorner=
      {
         prim=box;
         pos={75.0-2.6161, 65.0-2.6161, 2.5};
         rotation={0,0,45};
         size={9.899, 2.5, 5.0};
      };
      rightBackCorner=
      {
         prim=box;
         pos={75.0-2.6161, -65.0+2.6161, 2.5};
         rotation={0,0,-45};
         size={9.899, 2.5, 5.0};
      };
      leftBackGoalLateral=
      {
         prim=box;
         pos={80.0, 21.25, 2.5};
         size={10, 2.5, 5.0};
      };
      rightBackGoalLateral=
      {
         prim=box;
         pos={80.0, -21.25, 2.5};
         size={10, 2.5, 5.0};
      };
      goalBack=
      {
         prim=box;
         pos={85.0-1.25, 0, 2.5};
         size={2.5, 40, 5.0};
      };
   };
   material=
   {
      renderMaterial='texture { lateralMaterial }';
      color={0.9,0.8,0.6,1};
      bouncyness=0.5;
      minVelForBounce=0.1;
   }
};

field=
{
   objects={
      side1={ 
         objects={fieldSide};
      };
      side2={ 
         objects={fieldSide};
         rotation={0,0,180}; 
      };
      ground=
      {
         objects=
         {
            fieldGround={
               prim=box;
               pos={0,0,-1.25};
               size={150,130,2.5};
               material={ renderMaterial='groundMaterial(true)'; };
            };
            goal1Ground=
            {
               prim=box;
               pos={80,0,-1.25};
               size={10,40,2.5};
               material={ class=goal1Class; };
            };
            goal2Ground=
            {
               prim=box;
               pos={-80,0,-1.25};
               size={10,40,2.5};
               material={ class=goal2Class; };
            };
         };
         material=
         {
            renderMaterial='groundMaterial(false)';
            class=groundClass;
            color={0.5,0.8,0.5,1};
            bouncyness=0.3;
            minVelForBounce=0.1;
            friction=noSlip;
            erp=1.2;
         };
      };
      -- [[
      ballLimbo=
      {
         prim=plane;
         plane={0,0,1,-5};
         material=
         {
            class=ballClass;
            --renderMaterial=[[pigment { checker color rgb 0.2, color rgb 0.6 scale 15 }]];
            --renderMaterial=[[texture { T_Chrome_2D normal { waves 0.05 frequency 5000.0 
            --scale 3000.0 phase clock} }]];
            color={0.3,0.4,0.2};
            bouncyness=2;
         };
      };
      -- ]] --
   };
};

robot=
{
   objects=
   {
      cover2=
      {
         prim=box;
         pos={0,0,7.5};
         --size={7.5,7.5,7.5};
         size={7.6,7.6,0.05};
         compose='cover';
         mass=0.001; -- Kg/10
         material=
         {
            renderMaterial='pigment { color rgb <0.6,0.5,0.8> } finish {reflection {0.2 }}';
            color={0.6,0.5,0.8,1};
            class=6;
            friction=noSlip;
            --color={0.5,0.5,0.8,1};
         };
      };
      cover1=
      {
         prim=box;
         pos={0,0,3.8};
         --size={7.5,7.5,7.5};
         size={7.5,7.5,7.4};
         compose='cover';
         mass=10;
         material=
         {
            renderObject='robotBody';
            color={1,1,1};
            --color={0.5,0.5,0.8,1};
         };
      };
      rightWheel=
      {
         prim=cylinder;
         pos={0,3.7,3.7};
         radius=3.7;
         length=0.4;
         mass=0.001;
         material=
         {
            renderMaterial=[[ pigment { checker color rgb 0, color rgb <1.8,1.8,0.2> scale 4 }
            finish {reflection {0.2 }}]];
            color={0.8,0.8,0.6};
            friction=noSlip;
         };
      };
      leftWheel=
      {
         prim=cylinder;
         pos={0,-3.7,3.7};
         radius=3.7;
         length=0.4;
         mass=0.001;
         material=
         {
            renderMaterial=[[ pigment { checker color rgb 0, color rgb <1.8,1.8,0.2> scale 4 }
            finish {reflection {0.2 }}]];
            color={0.8,0.5,0.6};
            friction=noSlip;
         };
      };
   };
   material={ class=team1Class; };
   joints=
   {
      leftWheelJoint=
      {
         prim=hinge;
         body1='cover';
         body2='leftWheel';
         -- anchor={0,-3.7,3.7};
         axis={0,1,0};
      };
      rightWheelJoint=
      {
         prim=hinge;
         body1='cover';
         body2='rightWheel';
         -- anchor={0,3.7,3.7};
         axis={0,1,0};
      };
   }
}


scene=
{
   gravity={0,0,-100}; -- cm/s^2
   scale=10;
   objects=
   {
      ball={
         prim=sphere;
         pos={0,0,2.1};
         radius=2.1;
         mass=0.05; -- 5g
         material={
            class=ballClass;
            --povrayObject='Sphere2';
            renderMaterial=[[texture{ pigment{color <1,0.6,0.2>} normal{ crackle 0.9 scale 0.24 }
                     finish{phong 0.8 phong_size 200} }]];
            color={1,0.6,0.2};
            friction=noSlip;
         };
      };
      --[[robot2={
         objects={robot};
         pos={20,20,0};
      };
      robot1={
         objects={robot};
         pos={36,-20,0};
      };]]--
      theField={
         objects={field};
      };
   };
   
   material={
      cfm=0.1;
      erp=1;
   };
   viewPoints=
   {
      corner1={
         pos={-110,-100,65};
         rotation={45,-35,0};
      };
      goal2={
         pos={135,0,100};
         rotation={180,-45,0};
      };
      corner2={
         pos={108.893, -111.465, 65};
         rotation={133, -32.6, 0};
      };
      skyView={
         pos={0,0,185};
         rotation={-90,-90,0};
      };
   };
   lights=
   {
      { pos={200,150,200}; color={0.3,0.3,0.3}; };
      { pos={200,-150,200}; color={0.3,0.3,0.3}; };
      { pos={-200,150,200}; color={0.3,0.3,0.3}; };
      { pos={-200,-150,200}; color={0.3,0.3,0.3}; };
   };
   render=
[[
#include "colors.inc"
#include "metals.inc"
#include "skies.inc"

background { color rgb <0.6,0.6,0.9> }

//#declare firaColors=true;
#declare firaColors=false;
#declare noMarks=false;

#declare lateralMaterial=
#if (firaColors=true)
   texture{
      pigment { 
         checker
         color rgb 0
         color rgb 1
      }
      finish { specular 0.4 }
      scale 1000
      translate <500,500,2.49>
   }
#else
   texture{
      pigment { color rgb <0.9,0.8,0.6> } finish {reflection {0.1 metallic}}
   }
#end

#macro groundMaterial(marks)
   texture {
      pigment { 
         #if (firaColors=true)
            color rgb 0
         #else
            checker 
            color rgb <0.2,0.7,0.2>, color rgb <0.2,0.8,0.2> 
            scale 2 
         #end
      }
      #if (firaColors=true)
         finish { specular 0.4 }
      #end
   }
   #if (marks=true & noMarks=false)
      texture {
         pigment { 
            image_map { "firaMarks" once } 
            scale <155, 135, 1> 
            translate <-77.5,-67.5> 
         }
         finish { diffuse 1 }
      }
   #end
#end
   
#declare robotBody=/*union {
  light_source {
    <-0.6, 0, 0.25>
    color rgb 2
    spotlight
    radius 5
    falloff 30
    tightness 1
    point_at <-1.6, 0, 0.25>
  }
*/difference {

   box { <-1,-1,-1>, <1,1,1>
      //pigment { color rgb <2,2,2> } finish {reflection {0.1 }} 
      pigment { color rgb <0.6,0.6,0.6> } finish {reflection {0.1 }} 
      //texture { T_Chrome_2B }
   }
   torus { 0.23, 0.1 rotate 90*z translate -1*x translate 0.5*z
      pigment { color rgb 1 } finish {reflection {0.3 } ambient 0.7}
      //pigment { color rgb 0 } finish {reflection {0.3 }}
      //pigment { color rgb <0.6,0.5,0.8> } finish {reflection {0.2 }}
      //pigment { color rgb <1.5,0.5,0.5> } finish {reflection {0.2 }}
   }
   scale 0.5
}
//}
]];
};

function copyTable(src)
   local dest = { }
   return appendTable(dest,src)
end

function appendTable(dest,src)
   local i, v = next(src, nil)
   while i do
      if dest[i] == nil
      then 
         if type(v) == "table"
         then
            dest[i] = copyTable(v)
         else
            dest[i] = v
         end
      end
      i, v = next(src, i)
   end
   return dest
end

teamColors={
   { renderMaterial='pigment { color rgb <0.6,0.5,0.8> } finish {reflection {0.2 }}';
     color={0.6,0.5,0.8,1}; },
   { renderMaterial='pigment { color rgb <0.8,0.5,0.6> } finish {reflection {0.2 }}';
     color={0.8,0.5,0.6,1}; },
   { renderMaterial='pigment { color rgb <0.6,0.8,0.5> } finish {reflection {0.2 }}';
     color={0.6,0.8,0.5,1}; },
};

function setRobot(num, team, x, y, rot)
   scene.objects['robot'..num]={pos={x,y,0}; rotation={0,0,rot}; objects={}; };
   scene.objects['robot'..num].objects[1]=copyTable(robot);
   scene.objects['robot'..num].objects[1].objects.cover2.material.renderMaterial=
      teamColors[team].renderMaterial;
   scene.objects['robot'..num].objects[1].objects.cover2.material.color=teamColors[team].color;
end

setRobot(1,1,20,0,0);
--setRobot(2,2,36,-20,0);
--setRobot(3,1,-20,-20,30);
--setRobot(4,2,-36,20,-30);

config=
{
   maxVel=8;
   maxForce=180;
   ballVelDecay=0.005;
};

control=
{
   waitConnection=true;
   startDelay=30;
   --logName='teste.log';
   
   stepDuration=0.001;
   frameDuration=1/2;
   viewerDelay=0.1;
}

