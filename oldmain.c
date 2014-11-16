#include<stdio.h>
#include<stdlib.h>
#include<SDL/SDL.h>
#include<SDL/SDL_opengl.h>
#include<GL/gl.h>
#include<GL/glu.h>
#include<math.h>

int running=1;

typedef struct
{
float x,y;
float mass;
float moment_of_inertia;
float rotation;
}Body;


typedef struct
{
unsigned int VBO;
unsigned int IBO;
}Mesh;

typedef struct
{
Mesh mesh;
float mass;
float radius;
}Planet;

typedef struct
{
GLfloat x,y;
}Vertex;

Mesh CreateCircle(radius)
{
int numpoints=10;
int numpointsplus1=numpoints+1;
Vertex* vertices=malloc(sizeof(Vertex)*numpointsplus1);
GLushort* indices=malloc(sizeof(GLushort)*numpointsplus1);

//Vertex at centre of circle
vertices[0].x=0;
vertices[0].y=0;
indices[0]=0;
//Angle between successive vertices
float step=2*M_PI/numpoints;

int i;
float angle=0;
    for(i=1;i<numpointsplus1;i++)
    {
    vertices[i].x=sin(angle)*radius;
    vertices[i].y=cos(angle)*radius;
    indices[i]=i;
    angle+=2*M_PI;
    }

Mesh mesh;
mesh.VBO=glGenBuffers(1,&(mesh.VBO));
mesh.IBO=glGenBuffers(1,&(mesh.IBO));
glBindBuffer(GL_ARRAY_BUFFER,mesh.VBO);
glBufferData(GL_ARRAY_BUFFER,numpointsplus1*sizeof(Vertex),vertices,GL_STATIC_DRAW);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,mesh.IBO);
glBufferData(GL_ELEMENT_ARRAY_BUFFER,numpointsplus1*sizeof(GLushort),indices,GL_STATIC_DRAW);
free(vertices);
free(indices);
return mesh;
}

Planet CreatePlanet(float mass,float radius)
{
Planet planet;
planet.mass=mass;
planet.radius=radius;
planet.mesh=CreateCircle(radius);
return planet;
}


SDL_Surface* Init(int Width,int Height)
{
//Init SDL
if(SDL_Init(SDL_INIT_EVERYTHING)<0)return NULL;
//Find aspect ratio
double AspectRatio=(double)Width/(double)Height;
//Init video
SDL_Surface* DisplaySurface=SDL_SetVideoMode(Width,Height,32,SDL_HWSURFACE|SDL_OPENGL);
SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
if(DisplaySurface==NULL)return NULL;
//Init OpenGL
glClearColor(0.0,0.0,0.0,0.0);
glEnable(GL_DEPTH_TEST);
glMatrixMode(GL_PROJECTION);
glLoadIdentity();
glOrtho(-10*AspectRatio,10*AspectRatio,-10,10,-1,255);

return DisplaySurface;
}

void ProcessEvents()
{
SDL_Event Event;
while(SDL_PollEvent(&Event))
{
if(Event.type==SDL_QUIT)
{
running=0;
}
}
}

void GetDeltaT()
{
static unsigned int lastmilliseconds=0;

unsigned int curmilliseconds=SDL_GetTicks();
double deltaT=(double)(curmilliseconds-lastmilliseconds)/1000;
lastmilliseconds=curmilliseconds;
return deltaT;
}


void RenderScreen(Mesh mesh)
{
glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
glBegin(GL_QUADS);
glBindBuffer(GL_VERTEX_ARRAY,mesh.VBO);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,mesh.IBO);

glEnd();
glFinish();
SDL_GL_SwapBuffers();
}

void Quit()
{
SDL_Quit();
}

int main()
{

Init(640,480);

    while(running)
    {
    ProcessEvents();
    RenderScreen();
    }

Quit();
return 0;
}
