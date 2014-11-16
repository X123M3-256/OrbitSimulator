#ifndef SIM_INCLUDED
#define SIM_INCLUDED
#include "draw.h"

#define GRAVITATIONAL_CONSTANT 6.67e-11

#define DELTA_T 0.02
#define MAX_DELTA_T 100
#define MAX_THRUSTING_DELTA_T 1

#define MAX_OBJECTS 64
#define OBJECT_MAX_POINTS 32

typedef struct
{
double x,y;
}vector_t;

typedef struct
{
vector_t position;
vector_t offset;
double rotation;
double scale;
}camera_t;

typedef struct
{
vector_t position;
vector_t velocity;
vector_t acceleration;
vector_t force;
vector_t points[OBJECT_MAX_POINTS];
double rotation;
double delta_rot;
double mass;
int num_points;
}object_t;

struct celestial_body_s;

typedef struct
{
struct celestial_body_s* primary;
double semi_latus_rectum;
double semi_major_axis;
double longitude_of_periapsis;
double eccentricity;
}orbit_t;

typedef struct celestial_body_s
{
object_t base;
const char* name;
orbit_t orbit;
double radius;
double sphere_of_influence;
}celestial_body_t;

struct spacecraft_s;

typedef struct
{
struct spacecraft_s* docked_spacecraft;
int docked_dock;
//Position relative to spacecraft
vector_t position;
double orientation;
}dock_t;


typedef struct spacecraft_s
{
object_t base;
orbit_t orbit;
dock_t docks[4];
struct spacecraft_s* parent;
double dry_mass;
double capacity;
double fuel;
double thrust;
double specific_impulse;
int num_docks;
short flags;
short firing;
}spacecraft_t;


typedef struct
{
spacecraft_t* current_spacecraft;
camera_t camera;
int speedup;
celestial_body_t* primary;
celestial_body_t celestial_bodies[MAX_OBJECTS];
spacecraft_t spacecraft[MAX_OBJECTS];
int num_celestial_bodies;
int num_spacecraft;
}simulation_t;





void simulation_create(simulation_t* sim);
celestial_body_t* simulation_new_body(simulation_t* sim);
void simulation_render(simulation_t* sim);


void main_loop();

#endif
