#include "projects/world1/keyboard_mouse.h"

int highStreetA = loadImage("projects/world1/highStreetA.jpeg");
int highStreetB = loadImage("projects/world1/highStreetB.jpeg");
int highStreetC = loadImage("projects/world1/highStreetC.jpeg");
int highStreetD = loadImage("projects/world1/highStreetD.jpeg");
int rainbow = loadImage("projects/world1/rainbow.png");
int camImg = loadImage("projects/world1/cameraA.png");

int noiseID = loadSound("projects/world1/noise.flac");
int songID = loadSound("projects/world1/rainbow.mp3");
playSound(noiseID);
loopSound(noiseID);

moveImageVerts(highStreetA, 0, 0, 800);
int i;
for (i=0; i < 1024*1024; i++) // fuzzy tv signal effect
{
   setVertColor(highStreetA, i, 1, 1, 1);
   i += rand()%100;
}

moveImageVerts(highStreetB, 0, 0, -800);
rotImageVerts(highStreetC, 0.0, 90.0, 0.0);
rotImageVerts(highStreetD, 0.0, 90.0, 0.0);
moveImageVerts(highStreetC, 800, 0, 0);
moveImageVerts(highStreetD, -800, 0, 0);

scaleImageVerts(rainbow, 3, 3, 3);
rotImageVerts(camImg, 0.0, 90.0, 0.0);

loadObjToGPU(highStreetA);
loadObjToGPU(highStreetB);
loadObjToGPU(highStreetC);
loadObjToGPU(highStreetD);
loadObjToGPU(rainbow);
loadObjToGPU(camImg);

setObjLocZ(rainbow, -1600);

int textA = loadString("This code can be changed and ran without compiling!", 35, 50, 0.75, 0.4, 1.0, 0.4);
moveImageVerts(textA, -650, 0, 0);
loadObjToGPU(textA);
setType(textA, LINES_OBJECT);
setPointSize(textA, 3.0);

while (keyPress(KEY_ESCAPE) == FALSE)
{
   clearScreen(1.0, 1.0, 1.0);
   
   float turnUpDwn = getObjRotX(camImg);
   float turnLR = getObjRotY(camImg);
   float moveFwdBwd = 0.0;
   float moveLR = 0.0;
   float xTravDist, yTravDist, zTravDist;
   
   if (keyPress(KEY_LEFT))
   turnLR -= 6.8;
   
   if (keyPress(KEY_RIGHT))
   turnLR += 6.8;

   if (keyPress(KEY_UP))
   moveFwdBwd = 15.8;
   
   if (keyPress(KEY_DOWN))
   moveFwdBwd = -15.8;

   if (!keyPress(KEY_SPACE))
   {
      setCamRotX(getObjRotX(camImg));
      setCamRotY(getObjRotY(camImg));
      setCamRotZ(getObjRotZ(camImg));
      
      setCamLocX(getObjLocX(camImg));
      setCamLocY(getObjLocY(camImg));
      setCamLocZ(getObjLocZ(camImg));
   }
   
   // find the travel distance that can be added to the current location
   xTravDist = moveFwdBwd * sin(turnLR * (M_PI / 180.0));
   zTravDist = -moveFwdBwd * cos(turnLR * (M_PI / 180.0));
   
   // move back and forth
   setObjLocX(camImg, getObjLocX(camImg) + xTravDist);
   setObjLocZ(camImg, getObjLocZ(camImg) + zTravDist);
   
   // find the travel distance that can be added to the current location
   turnLR += 90;
   xTravDist = moveLR * sin(turnLR * (M_PI / 180.0));
   zTravDist = -moveLR * cos(turnLR * (M_PI / 180.0));
   turnLR -= 90;
   
   // move left and right
   setObjLocX(camImg, getObjLocX(camImg) + xTravDist);
   setObjLocZ(camImg, getObjLocZ(camImg) + zTravDist);
   
   // rotate object to the direction of travel
   setObjRotY(camImg, turnLR);
   
   // create 3D sound effect
   float x1, z1, x2, z2;
   float distance;
   x1 = 0.0;
   z1 = 800.0;
   x2 = getObjLocX(camImg);
   z2 = getObjLocZ(camImg);
   distance = sqrt((x2-x1)*(x2-x1) + (z2-z1)*(z2-z1));
   
   // change volume determined by distance
   if (distance < 3000)
   soundVolume(noiseID, 1.0 - distance/3000);
   
   // pan sound determined by rotation
   if (getObjLocZ(camImg) < 800.0)
   panSound(noiseID, sin(turnLR * (M_PI / 180.0)) * 0.8 );
   else
   panSound(noiseID, -sin(turnLR * (M_PI / 180.0)) * 0.8 );
   
   // load song if past rainbow
   if (getObjLocZ(camImg) < getObjLocZ(rainbow))
   playSound(songID);
   
   paintScreen();
   displayScreen();
}
