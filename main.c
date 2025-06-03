#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <GL/glew.h> // include GLEW before GLFW
#include <GLFW/glfw3.h>
#include <sys/time.h>
#include <unistd.h> // for usleep
#include "loadImg.h"
#include "ascii.h"
#include "audio.h"

#include "picoc_modified/picoc.h"

#define TRUE 1
#define FALSE 0

const char* WINDOW_TITLE = "sandbox";
GLFWwindow* window;
float x_scroll_wheel = 0;
float y_scroll_wheel = 0;

// set view clipping, perspective, fog
int width = 800, height = 600;
float viewStart = 500;
float viewEnd = -500;
float persp = 410;
float persp_save = 410;
int orth = FALSE;
int unitPerPix = TRUE; // will move the lens until "1 unit" == "1 pixel"
float zMov, zSz;

float fogColor[3] = {0.0, 0.0, 0.0};
float fogStart = 100.0;
float fogEnd = -1000.0;

float camera_td[10] = {0};

// setup shader variables
GLuint shaderProgram;
GLint viewMemLoc;
GLint winMemLoc;
GLint fogStartMemLoc;
GLint fogEndMemLoc; 
GLint fogColorMemLoc;
GLint transformCamLoc;
GLint transformObjLoc;
GLint pointSizeMemLoc;


struct vert_obj {
   float *verts;
   float *vCols;
   int vertCnt;
   float ptSize;
   float td[10]; // transformation data
   int type;
   int hide;
};

// Vertex Shader
const char *vertexShaderSource = R"glsl(
   #version 330 core
   layout (location = 0) in vec3 vertPosition;
   layout(location = 1) in vec3 vertColor;
   out vec3 vColor;

   uniform vec2 win;
   uniform vec3 view;
   uniform float pointSize;
   
   uniform float transformObj[10];
   uniform float transformCam[10];
   
   // rotate using unit circle coordinates
   void ucRot(float hUc, float vUc, inout float hP, inout float vP)
   {
      float h = vP * -vUc + hP * hUc;
      float v = hP * vUc + vP * hUc;
      hP = h;
      vP = v;
   }

   void main()
   {
      float persp = 1.0;
      float x = vertPosition.x;
      float y = vertPosition.y;
      float z = vertPosition.z;
      
      vColor = vertColor;
      
      // set object scale
      x *= transformObj[0];
      y *= transformObj[0];
      z *= transformObj[0];
      
      // set object x,y,z rotation
      ucRot(transformObj[1], transformObj[2], y, z);
      ucRot(transformObj[3], transformObj[4], x, z);
      ucRot(transformObj[5], transformObj[6], x, y);
      
      // set object location
      x += transformObj[7];
      y += transformObj[8];
      z += transformObj[9];
      
      
      // set camera location
      x -= transformCam[7];
      y -= transformCam[8];
      z -= transformCam[9];
      
      // set camera x,y,z rotation
      ucRot(transformCam[5], -transformCam[6], x, y);
      ucRot(transformCam[3], -transformCam[4], x, z);
      ucRot(transformCam[1], -transformCam[2], y, z);
      
      // set camera scale
      if (transformCam[0] != 0.0)
      {
         x /= transformCam[0];
         y /= transformCam[0];
         z /= transformCam[0];
      }
      
      
      if (view[0] != 0.0) // if not in orthographic mode
      persp = (view[0] - (z - view[1])) / view[0];

      gl_Position = vec4(x*2/win.x, y*2/win.y,(z - view[1]) / view[2], persp);
      
      gl_PointSize = pointSize;
   }
)glsl";

// Fragment Shader
const char *fragmentShaderSource = R"glsl(
   #version 330 core
   in vec3 vColor;
   out vec4 FragColor;

   uniform vec2 win;
   uniform vec3 view;
   uniform vec3 fogColor;
   uniform float fogStart;
   uniform float fogEnd;

   void main()
   {
      float zGl, z;
      float x, y;
      
      // retrieve the fragment's position in view space
      vec4 fragmentPosition = gl_FragCoord;
      
      // reverse the format and prospective to find the z position
      zGl = fragmentPosition.z * 2.0 - 1.0;
      
      if (view[0] != 0.0) // if not in orthographic mode
      z = (zGl * view[0] * view[2] + zGl * view[1] * view[2] + view[0] * view[1]) / (zGl * view[2] + view[0]);
      else
      z = zGl * view[2] + view[1];
      
      // find the x and y positions
      x = fragmentPosition.x - 0.5 * win.x;
      y = fragmentPosition.y - 0.5 * win.y;
      
      if (view[0] != 0.0) // if not in orthographic mode
      {
         x *= (view[0] - (z - view[1])) / view[0];
         y *= (view[0] - (z - view[1])) / view[0];
      }
      
      // add fog
      float fogAlpha = 0.0;
      vec3 colorFogMix;
      
      if (z < fogStart)
      fogAlpha = 1.0 - (z - fogEnd) / (fogStart - fogEnd);
      
      if (z < fogEnd)
      fogAlpha = 1.0;
      
      colorFogMix = mix(vColor, fogColor, fogAlpha);
      
      FragColor = vec4(colorFogMix, 1.0);
   }
)glsl";


void setCameraLens(float viewStart, float viewEnd, int orthoMode, int unitPerPix,
                   float *perspect, float *zMov, float *zSz)
{
	float viewDist, d, p, clipFront;

	viewDist = viewStart - viewEnd;

	if (orthoMode) // orthographic perspective
	{
		*zSz = viewDist / 2;
		*zMov = (viewStart + viewEnd) / 2;
		*perspect = 0; // tells the renderer to not use variable
	}
	else
	{
		// inverse of f(x) = y / (y + x) * x + y / (y - x) * x
		d = viewDist * viewDist;
		p = *perspect * *perspect;
		*zSz = (sqrt(p*p + p*d) - p) / viewDist;

		clipFront = *perspect / (*perspect + *zSz) * *zSz;
		*zMov = viewStart - clipFront;

		// one unit per pixel if z == 0
		if (unitPerPix) // view is moved such that one unit == one pixel
		*zMov = 0;
	}
}

void clearScreen(float r, float g, float b)
{
   glClearColor(r, g, b, 1.0f);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

unsigned int *VAO = 0;
unsigned int *VBO_vertices = 0;
unsigned int *VBO_colors = 0;
struct vert_obj *vObj_arr = 0;
int vObj_cnt = 0;
void paintScreen()
{
   int i;
   
   // activate shader program
   glUseProgram(shaderProgram);
   
   // window settings
   glfwGetWindowSize(window, &width, &height);
   glUniform2f(winMemLoc, width, height);
   
   // view settings
   glUniform3f(viewMemLoc, persp, zMov, zSz);
   
   // fog settings
   glUniform3fv(fogColorMemLoc, 1, fogColor);
   glUniform1f(fogStartMemLoc, fogStart);
   glUniform1f(fogEndMemLoc, fogEnd);
   

   // add camera transformation data
   glUniform1fv(transformCamLoc, 10, camera_td);
   
   for (i=0; i < vObj_cnt; i++)
   {
      if (vObj_arr[i].hide) // set visibility
      continue;
      
      // set point size and line thickness
      glUniform1f(pointSizeMemLoc, vObj_arr[i].ptSize);
      glLineWidth(vObj_arr[i].ptSize);
      
      glUniform1fv(transformObjLoc, 10, vObj_arr[i].td); // add object transformation data
      
      glBindVertexArray(VAO[i]); // bind the VAO
      
      if (vObj_arr[i].type == 0)
      glDrawArrays(GL_POINTS, 0, vObj_arr[i].vertCnt);
      else
      glDrawArrays(GL_LINES, 0, vObj_arr[i].vertCnt);
      
      glBindVertexArray(0); // unbind the VAO
   }
}

void displayScreen()
{
   // set FPS timer variables
   #define FPS_SETTING 20
   static int sleep_time = 1000000 / FPS_SETTING; // convert to microseconds
   static struct timeval start = {0};
   static struct timeval end = {0};
   static int frameCount = 0;
   
   if (!start.tv_sec)
   gettimeofday(&start, NULL); // get the initial time
   
   usleep(sleep_time); // FPS limiter
   
   // calculate the elapsed time in seconds
   frameCount++;
   gettimeofday(&end, NULL); // get the current time
   double elapsedTime = (end.tv_sec - start.tv_sec) + 
                        (end.tv_usec - start.tv_usec) / 1000000.0;

   // update FPS every second
   if (elapsedTime >= 1.0)
   {
      //printf("FPS: %d\n", frameCount);
      //printf("\033[A"); // move cursor up one line
      frameCount = 0;
      start = end; // reset start time
   }
   
   glfwSwapBuffers(window); // swap buffers
   glfwPollEvents(); // poll events
}

int keyPress(int key)
{
   if (glfwGetKey(window, key) == GLFW_PRESS)
   return TRUE;
   else
   return FALSE;
}
int mouseClick(int button)
{
   if (glfwGetMouseButton(window, button) == GLFW_PRESS)
   return TRUE;
   else
   return FALSE;
}
void getCursorPosition(double *xpos, double *ypos)
{
   glfwGetCursorPos(window, xpos, ypos);
}
void getScrollWheel(double *xscroll, double *yscroll)
{
   *xscroll = x_scroll_wheel;
   *yscroll = y_scroll_wheel;
}

int forceShutdown()
{
   glfwPollEvents(); // poll events
   
   if (glfwWindowShouldClose(window))
   return TRUE;
   
   return FALSE;
}


void setType(unsigned int id, int type)
{
   if (id >= vObj_cnt) //check bounds
   return;
   
   vObj_arr[id].type = type;
}
void setHide(unsigned int id, int state)
{
   if (id >= vObj_cnt) //check bounds
   return;
   
   vObj_arr[id].hide = state;
}

void setPointSize(unsigned int id, float size)
{
   if (id >= vObj_cnt) //check bounds
   return;
   
   vObj_arr[id].ptSize = size;
}

void setObjScale(unsigned int id, float scale)
{
   if (id >= vObj_cnt) //check bounds
   return;
   
   vObj_arr[id].td[0] = scale;
}

void setObjRotX(unsigned int id, float deg)
{
   if (id >= vObj_cnt) //check bounds
   return;
   
   float rad = deg * M_PI / 180.0;
   float h = cos(rad);
   float v = sin(rad);
   
   vObj_arr[id].td[1] = h;
   vObj_arr[id].td[2] = v;
}

void setObjRotY(unsigned int id, float deg)
{
   if (id >= vObj_cnt) //check bounds
   return;
   
   float rad = deg * M_PI / 180.0;
   float h = cos(rad);
   float v = sin(rad);
   
   vObj_arr[id].td[3] = h;
   vObj_arr[id].td[4] = v;
}

void setObjRotZ(unsigned int id, float deg)
{
   if (id >= vObj_cnt) //check bounds
   return;
   
   float rad = deg * M_PI / 180.0;
   float h = cos(rad);
   float v = sin(rad);
   
   vObj_arr[id].td[5] = h;
   vObj_arr[id].td[6] = v;
}

void setObjLocX(unsigned int id, float loc)
{
   if (id >= vObj_cnt) //check bounds
   return;
   
   vObj_arr[id].td[7] = loc;
}

void setObjLocY(unsigned int id, float loc)
{
   if (id >= vObj_cnt) //check bounds
   return;
   
   vObj_arr[id].td[8] = loc;
}

void setObjLocZ(unsigned int id, float loc)
{
   if (id >= vObj_cnt) //check bounds
   return;
   
   vObj_arr[id].td[9] = loc;
}


float getObjScale(unsigned int id)
{
   if (id >= vObj_cnt) //check bounds
   return 0;
   
   return vObj_arr[id].td[0];
}
float getObjRotX(unsigned int id)
{
   if (id >= vObj_cnt) //check bounds
   return 0;
   
   if (vObj_arr[id].td[2] > 0)
   return acos(vObj_arr[id].td[1]) * 180.0 / M_PI;
   else
   return -acos(vObj_arr[id].td[1]) * 180.0 / M_PI;
}
float getObjRotY(unsigned int id)
{
   if (id >= vObj_cnt) //check bounds
   return 0;
   
   if (vObj_arr[id].td[4] > 0)
   return acos(vObj_arr[id].td[3]) * 180.0 / M_PI;
   else
   return -acos(vObj_arr[id].td[3]) * 180.0 / M_PI;
}
float getObjRotZ(unsigned int id)
{
   if (id >= vObj_cnt) //check bounds
   return 0;
   
   if (vObj_arr[id].td[6] > 0)
   return acos(vObj_arr[id].td[5]) * 180.0 / M_PI;
   else
   return -acos(vObj_arr[id].td[5]) * 180.0 / M_PI;
}
float getObjLocX(unsigned int id)
{
   if (id >= vObj_cnt) //check bounds
   return 0;
   
   return vObj_arr[id].td[7];
}
float getObjLocY(unsigned int id)
{
   if (id >= vObj_cnt) //check bounds
   return 0;
   
   return vObj_arr[id].td[8];
}
float getObjLocZ(unsigned int id)
{
   if (id >= vObj_cnt) //check bounds
   return 0;
   
   return vObj_arr[id].td[9];
}


void setCamScale(float scale)
{
   camera_td[0] = scale;
}
void setCamRotX(float deg)
{
   float rad = deg * M_PI / 180.0;
   float h = cos(rad);
   float v = sin(rad);
   
   camera_td[1] = h;
   camera_td[2] = v;
}
void setCamRotY(float deg)
{
   float rad = deg * M_PI / 180.0;
   float h = cos(rad);
   float v = sin(rad);
   
   camera_td[3] = h;
   camera_td[4] = v;
}
void setCamRotZ(float deg)
{
   float rad = deg * M_PI / 180.0;
   float h = cos(rad);
   float v = sin(rad);
   
   camera_td[5] = h;
   camera_td[6] = v;
}
void setCamLocX(float loc)
{
   camera_td[7] = loc;
}
void setCamLocY(float loc)
{
   camera_td[8] = loc;
}
void setCamLocZ(float loc)
{
   camera_td[9] = loc;
}

float getCamScale()
{
   return camera_td[0];
}
float getCamRotX()
{
   if (camera_td[2] > 0)
   return acos(camera_td[1]) * 180.0 / M_PI;
   else
   return -acos(camera_td[1]) * 180.0 / M_PI;
}
float getCamRotY()
{
   if (camera_td[4] > 0)
   return acos(camera_td[3]) * 180.0 / M_PI;
   else
   return -acos(camera_td[3]) * 180.0 / M_PI;
}
float getCamRotZ()
{
   if (camera_td[6] > 0)
   return acos(camera_td[5]) * 180.0 / M_PI;
   else
   return -acos(camera_td[5]) * 180.0 / M_PI;
}
float getCamLocX()
{
   return camera_td[7];
}
float getCamLocY()
{
   return camera_td[8];
}
float getCamLocZ()
{
   return camera_td[9];
}


void setViewStart(float value)
{
   viewStart = value;
   setCameraLens(viewStart, viewEnd, orth, unitPerPix, &persp, &zMov, &zSz);
}
void setViewEnd(float value)
{
   viewEnd = value;
   setCameraLens(viewStart, viewEnd, orth, unitPerPix, &persp, &zMov, &zSz);
}
void setPerspective(float value)
{
   persp = value;
   persp_save = persp;
   setCameraLens(viewStart, viewEnd, orth, unitPerPix, &persp, &zMov, &zSz);
}
void orthographicMode(int state)
{
   if (persp)
   persp_save = persp;
   else
   persp = persp_save;
   
   orth = state;
   setCameraLens(viewStart, viewEnd, orth, unitPerPix, &persp, &zMov, &zSz);
}
void unitPerPixelMode(int state)
{
   unitPerPix = state;
   setCameraLens(viewStart, viewEnd, orth, unitPerPix, &persp, &zMov, &zSz);
}
void setFogColor(float r, float g, float b)
{
   fogColor[0] = r;
   fogColor[1] = g;
   fogColor[2] = b;
}
void setFogStart(float value)
{
   fogStart = value;
}
void setFogEnd(float value)
{
   fogEnd = value;
}

float getWindowWidth()
{
   return width;
}
float getWindowHeight()
{
   return height;
}
float getViewStart()
{
   return viewStart;
}
float getViewEnd()
{
   return viewEnd;
}
float getPerspective()
{
   return persp;
}
void getFogColor(double *r, double *g, double *b)
{
   *r = fogColor[0];
   *g = fogColor[1];
   *b = fogColor[2];
}
float getFogStart()
{
   return fogStart;
}
float getFogEnd()
{
   return fogEnd;
}


int loadImage(char *filename)
{
   // create room for new vertice object
   struct vert_obj *vObj_tmp;
   
   // allocate memory for tmp and copy arr to tmp
   vObj_tmp = malloc((vObj_cnt+1) * sizeof(struct vert_obj));
   memcpy(vObj_tmp, vObj_arr, vObj_cnt * sizeof(struct vert_obj));
   
   // assign arr pointer to the new memory location
   if (vObj_arr)
   free(vObj_arr);
   
   vObj_arr = vObj_tmp;
   memset(&vObj_arr[vObj_cnt], 0, sizeof(struct vert_obj));
   
   if (loadImg(filename, &vObj_arr[vObj_cnt].verts, &vObj_arr[vObj_cnt].vCols, &vObj_arr[vObj_cnt].vertCnt))
   {
      vObj_arr = realloc(vObj_arr, vObj_cnt * sizeof(struct vert_obj));
      return -1;
   }
   
   // create room for a new VAO and VBOs 
   unsigned int *VAO_tmp;
   unsigned int *VBO_vertices_tmp;
   unsigned int *VBO_colors_tmp;
   
   // allocate memory for tempory arrays and copy arrays to tempory arrays
   VAO_tmp = malloc((vObj_cnt+1) * sizeof(unsigned int));
   VBO_vertices_tmp = malloc((vObj_cnt+1) * sizeof(unsigned int));
   VBO_colors_tmp = malloc((vObj_cnt+1) * sizeof(unsigned int));
   memcpy(VAO_tmp, VAO, vObj_cnt * sizeof(unsigned int));
   memcpy(VBO_vertices_tmp, VBO_vertices, vObj_cnt * sizeof(unsigned int));
   memcpy(VBO_colors_tmp, VBO_colors, vObj_cnt * sizeof(unsigned int));
   
   // assign pointers to new memory locations
   if (VAO)
   free(VAO);
   
   if (VBO_vertices)
   free(VBO_vertices);
   
   if (VBO_colors)
   free(VBO_colors);
   
   VAO = VAO_tmp;
   VBO_vertices = VBO_vertices_tmp;
   VBO_colors = VBO_colors_tmp;
   
   vObj_cnt++;
   
   // update transformation data
   vObj_arr[vObj_cnt-1].ptSize = 1;
   setObjScale(vObj_cnt-1, 1.0);
   setObjRotX(vObj_cnt-1, 0.0);
   setObjRotY(vObj_cnt-1, 0.0);
   setObjRotZ(vObj_cnt-1, 0.0);
   
   return vObj_cnt - 1;
}
int loadVerts(double *verts, double *vCols, int vertCnt)
{
   int i;
   
   // create room for new vertice object
   struct vert_obj *vObj_tmp;
   
   // allocate memory for tmp and copy arr to tmp
   vObj_tmp = malloc((vObj_cnt+1) * sizeof(struct vert_obj));
   memcpy(vObj_tmp, vObj_arr, vObj_cnt * sizeof(struct vert_obj));
   
   // assign arr pointer to the new memory location
   if (vObj_arr)
   free(vObj_arr);
   
   vObj_arr = vObj_tmp;
   memset(&vObj_arr[vObj_cnt], 0, sizeof(struct vert_obj));
   
   if (!vertCnt) // delete if no verts
   {
      vObj_arr = realloc(vObj_arr, vObj_cnt * sizeof(struct vert_obj));
      return -1;
   }
   
   // copy verts
   float *verts_tmp, *vCols_tmp;
   vCols_tmp = malloc(vertCnt * sizeof(float) * 3);
   verts_tmp = malloc(vertCnt * sizeof(float) * 3);
   for (i=0; i < vertCnt*3; i++)
   {
      verts_tmp[i] = verts[i];
      vCols_tmp[i] = vCols[i];
   }
   vObj_arr[vObj_cnt].verts = verts_tmp;
   vObj_arr[vObj_cnt].vCols = vCols_tmp;
   vObj_arr[vObj_cnt].vertCnt = vertCnt;
   
   // create room for a new VAO and VBOs 
   unsigned int *VAO_tmp;
   unsigned int *VBO_vertices_tmp;
   unsigned int *VBO_colors_tmp;
   
   // allocate memory for tempory arrays and copy arrays to tempory arrays
   VAO_tmp = malloc((vObj_cnt+1) * sizeof(unsigned int));
   VBO_vertices_tmp = malloc((vObj_cnt+1) * sizeof(unsigned int));
   VBO_colors_tmp = malloc((vObj_cnt+1) * sizeof(unsigned int));
   memcpy(VAO_tmp, VAO, vObj_cnt * sizeof(unsigned int));
   memcpy(VBO_vertices_tmp, VBO_vertices, vObj_cnt * sizeof(unsigned int));
   memcpy(VBO_colors_tmp, VBO_colors, vObj_cnt * sizeof(unsigned int));
   
   // assign pointers to new memory locations
   if (VAO)
   free(VAO);
   
   if (VBO_vertices)
   free(VBO_vertices);
   
   if (VBO_colors)
   free(VBO_colors);
   
   VAO = VAO_tmp;
   VBO_vertices = VBO_vertices_tmp;
   VBO_colors = VBO_colors_tmp;
   
   vObj_cnt++;
   
   // update transformation data
   vObj_arr[vObj_cnt-1].ptSize = 1;
   setObjScale(vObj_cnt-1, 1.0);
   setObjRotX(vObj_cnt-1, 0.0);
   setObjRotY(vObj_cnt-1, 0.0);
   setObjRotZ(vObj_cnt-1, 0.0);
   
   return vObj_cnt - 1;
}
int loadString(char *text, float char_width, float char_height,
               float spacing, float r, float g, float b)
{
   #define MAX_STRING_SIZE (1024*1024)
   unsigned ch;
   int i, j, coorCnt, id;
   double *verts;
   double *vCols;
   
   int iVert = 0;
   int dataSz = 0;
   for (i=0; i < MAX_STRING_SIZE; i++)
   {
      ch = text[i];
      if (!ch)
      break;
      
      dataSz += (asciiLoc[ch+1] - asciiLoc[ch]) * sizeof(double);
   }
   
   verts = malloc(dataSz);
   vCols = malloc(dataSz);
   
   for (i=0; i < MAX_STRING_SIZE; i++)
   {
      ch = text[i];
      if (!ch)
      break;
      
      if (ch > 126)
      ch = 32;
      
      coorCnt = asciiLoc[ch+1] - asciiLoc[ch];
      
      for (j=asciiLoc[ch]; j < asciiLoc[ch]+coorCnt; j+=3)
      {
         verts[iVert] = asciiData[j] * char_width + (i * spacing) * char_width;
         verts[iVert+1] = asciiData[j+1] * char_height;
         verts[iVert+2] = asciiData[j+2];
         
         vCols[iVert] = r;
         vCols[iVert+1] = g;
         vCols[iVert+2] = b;
         iVert += 3;
      }
   }
   
   id = loadVerts(verts, vCols, dataSz/(sizeof(double)*3));
   
   free(verts);
   free(vCols);
   return id;
}

void loadObjToGPU(int id)
{
   if (id >= vObj_cnt) //check bounds
   return;
   
   glGenVertexArrays(1, &VAO[id]); // generate and bind Vertex Array Object
   glBindVertexArray(VAO[id]); // set configuration for this VAO

   glGenBuffers(1, &VBO_vertices[id]);// Vertex Buffer Object (VBO) for positions
   glBindBuffer(GL_ARRAY_BUFFER, VBO_vertices[id]); // Bind the buffer
   
   // copy vertex data into the buffer's memory
   glBufferData(GL_ARRAY_BUFFER, vObj_arr[id].vertCnt*3*sizeof(float), vObj_arr[id].verts, GL_STATIC_DRAW);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
   glEnableVertexAttribArray(0); // enable the vertex attribute

   glGenBuffers(1, &VBO_colors[id]); // Vertex Buffer Object (VBO) for colors
   glBindBuffer(GL_ARRAY_BUFFER, VBO_colors[id]); // bind the buffer
   
   // copy color data into the buffer's memory
   glBufferData(GL_ARRAY_BUFFER, vObj_arr[id].vertCnt*3*sizeof(float), vObj_arr[id].vCols, GL_STATIC_DRAW);
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
   glEnableVertexAttribArray(1); // enable the vertex attribute
   
   glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind the VBO
   glBindVertexArray(0); // unbind the VAO
}

// rotate using unit circle coordinates
void ucRot(float hUc, float vUc, float *hP, float *vP)
{
   float h = *vP * -vUc + *hP * hUc;
   float v = *hP * vUc + *vP * hUc;
   *hP = h;
   *vP = v;
}

void rotate(float *verts, int cnt, float x, float y, float z)
{
   int i;
   float xh, xv, yh, yv, zh, zv;

   x *= M_PI / 180.0;
   xh = cos(x);
   xv = sin(x);
   
   for (i=0; i < cnt; i+=3)
   ucRot(xh, xv, &verts[i+1], &verts[i+2]);

   y *= M_PI / 180.0;
   yh = cos(y);
   yv = sin(y);
   
   for (i=0; i < cnt; i+=3)
   ucRot(yh, yv, &verts[i+0], &verts[i+2]);

   z *= M_PI / 180.0;
   zh = cos(z);
   zv = sin(z);
   
   for (i=0; i < cnt; i+=3)
   ucRot(zh, zv, &verts[i+0], &verts[i+1]);
}

void rotImageVerts(unsigned int id, float x, float y, float z)
{
   if (id >= vObj_cnt) //check bounds
   return;
   
   rotate(vObj_arr[id].verts, vObj_arr[id].vertCnt*3, x, y, z);
}

void scaleImageVerts(unsigned int id, float x, float y, float z)
{
   if (id >= vObj_cnt) //check bounds
   return;
   
   int i;
   int cnt = vObj_arr[id].vertCnt * 3;
   
   for (i=0; i < cnt; i+=3)
   {
      vObj_arr[id].verts[i] *= x;
      vObj_arr[id].verts[i+1] *= y;
      vObj_arr[id].verts[i+2] *= z;
   }
}

void moveImageVerts(unsigned int id, float x, float y, float z)
{
   if (id >= vObj_cnt) //check bounds
   return;
   
   int i;
   int cnt = vObj_arr[id].vertCnt * 3;
   
   for (i=0; i < cnt; i+=3)
   {
      vObj_arr[id].verts[i] += x;
      vObj_arr[id].verts[i+1] += y;
      vObj_arr[id].verts[i+2] += z;
   }
}


void setVert(unsigned int id, unsigned int loc, float x, float y, float z)
{
   if (id >= vObj_cnt) //check bounds
   return;
   
   if (loc >= vObj_arr[id].vertCnt)
   return;
   
   unsigned int i = loc * 3;
   vObj_arr[id].verts[i] = x;
   vObj_arr[id].verts[i+1] = y;
   vObj_arr[id].verts[i+2] = z;
}
void setVertColor(unsigned int id, unsigned int loc, float r, float g, float b)
{
   if (id >= vObj_cnt) //check bounds
   return;
   
   if (loc >= vObj_arr[id].vertCnt)
   return;
   
   unsigned int i = loc * 3;
   vObj_arr[id].vCols[i] = r;
   vObj_arr[id].vCols[i+1] = g;
   vObj_arr[id].vCols[i+2] = b;
}
void getVert(unsigned int id, unsigned int loc, double *x, double *y, double *z)
{
   if (id >= vObj_cnt) //check bounds
   return;
   
   if (loc >= vObj_arr[id].vertCnt)
   return;
   
   unsigned int i = loc * 3;
   *x = vObj_arr[id].verts[i];
   *y = vObj_arr[id].verts[i+1];
   *z = vObj_arr[id].verts[i+2];
}
void getVertColor(unsigned int id, unsigned int loc, double *r, double *g, double *b)
{
   if (id >= vObj_cnt) //check bounds
   return;
   
   if (loc >= vObj_arr[id].vertCnt)
   return;
   
   unsigned int i = loc * 3;
   *r = vObj_arr[id].vCols[i];
   *g = vObj_arr[id].vCols[i+1];
   *b = vObj_arr[id].vCols[i+2];
}



// shader compilation helper
GLuint compileShader(GLenum type, const char* source, const char* shaderName)
{
   GLuint shader = glCreateShader(type);
   glShaderSource(shader, 1, &source, NULL);
   glCompileShader(shader);
   
   int success;
   char infoLog[512];
   glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
   if (!success)
   {
      glGetShaderInfoLog(shader, 512, NULL, infoLog);
      fprintf(stderr, "SHADER ERROR: %s, COMPILATION FAILED\n%s\n", shaderName, infoLog);
      glDeleteShader(shader);
      return 0;
   }
   
   return shader;
}

// shader program linking helper
GLuint linkShaderProgram(GLuint vertexShader, GLuint fragmentShader)
{
   GLuint shaderProgram = glCreateProgram();
   glAttachShader(shaderProgram, vertexShader);
   glAttachShader(shaderProgram, fragmentShader);
   glLinkProgram(shaderProgram);
   
   int success;
   char infoLog[512];
   glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
   if (!success)
   {
      glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
      fprintf(stderr, "PROGRAM ERROR: LINKING FAILED\n%s\n", infoLog);
      glDeleteProgram(shaderProgram);
      return 0;
   }
   
   return shaderProgram;
}

// GLFW error callback
void error_callback(int error, const char* description)
{
   fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

// GLFW framebuffer size callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
   glViewport(0, 0, width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
   x_scroll_wheel += xoffset;
   y_scroll_wheel += yoffset;
}

#include "picoc_interpreter.c"

int main()
{
   // initialize GLFW
   glfwSetErrorCallback(error_callback);
   if (!glfwInit())
   {
      fprintf(stderr, "Failed to initialize GLFW\n");
      return -1;
   }
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

   // create window
   window = glfwCreateWindow(width, height, WINDOW_TITLE, NULL, NULL);
   if (window == NULL)
   {
      fprintf(stderr, "Failed to create GLFW window\n");
      glfwTerminate();
      return -1;
   }
   glfwMakeContextCurrent(window);
   glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

   glfwSetScrollCallback(window, scroll_callback);
   
   // initialize GLEW
   GLenum err = glewInit();
   if (GLEW_OK != err)
   {
      fprintf(stderr, "Error initializing GLEW: %s\n", glewGetErrorString(err));
      glfwTerminate();
      return -1;
   }
   printf("OpenGL Version: %s\n", glGetString(GL_VERSION));

   // compile and link shaders
   GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource, "VERTEX");
   GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource, "FRAGMENT");
   if (vertexShader == 0 || fragmentShader == 0)
   {
      glfwTerminate();
      return -1;
   }
   shaderProgram = linkShaderProgram(vertexShader, fragmentShader);
   glDeleteShader(vertexShader);
   glDeleteShader(fragmentShader);
   if (shaderProgram == 0)
   {
      glfwTerminate();
      return -1;
   }

   // configure OpenGL state
	glClearDepth(-1.0);
	glDepthFunc(GL_GEQUAL);
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_PROGRAM_POINT_SIZE);

   // setup shader variables
   viewMemLoc = glGetUniformLocation(shaderProgram, "view");
   winMemLoc = glGetUniformLocation(shaderProgram, "win");
   fogStartMemLoc = glGetUniformLocation(shaderProgram, "fogStart");
   fogEndMemLoc = glGetUniformLocation(shaderProgram, "fogEnd");
   fogColorMemLoc = glGetUniformLocation(shaderProgram, "fogColor");
   transformCamLoc = glGetUniformLocation(shaderProgram, "transformCam");
   transformObjLoc = glGetUniformLocation(shaderProgram, "transformObj");
   pointSizeMemLoc = glGetUniformLocation(shaderProgram, "pointSize");
   
   // set up camera transformation data
   setCamScale(1.0);
   setCamRotX(0.0);
   setCamRotY(0.0);
   setCamRotZ(0.0);
   
   setCameraLens(viewStart, viewEnd, orth, unitPerPix, &persp, &zMov, &zSz);
   
   iniAudioEngine();

   int StackSize = 128 * 1024 * 1024; // space for the stack
   PicocInitialise(StackSize);
   PicocIncludeAllSystemHeaders();

   if (PicocPlatformSetExitPoint())
   {
      PicocCleanup();
      printf("Picoc Exit Value:%i\n", PicocExitValue);
   }
   else
   {
      PicocPlatformScanFile("projects/world1/main.c");
      
      PicocCleanup();
      printf("Picoc Exit Value:%i\n", PicocExitValue);
   }
   
   // free audio resources
   endAudioEngine();
   
   // free loaded objects
   int i;
   for (i=0; i < vObj_cnt; i++)
   freeImg(vObj_arr[i].verts, vObj_arr[i].vCols);
   
   if (vObj_arr)
   free(vObj_arr);
   
   if (VAO)
   free(VAO);
   if (VBO_vertices)
   free(VBO_vertices);
   if (VBO_colors)
   free(VBO_colors);
   
   glDeleteProgram(shaderProgram);

   glfwDestroyWindow(window);
   glfwTerminate();
   return 0;
}
