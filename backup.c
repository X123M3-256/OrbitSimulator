#include<stdlib.h>
#include<stdio.h>
#include<math.h>
#include "draw.h"
#include "keys.h"
#include "sim.h"
#include "stats.h"

#define OFFSET_X 750
#define OFFSET_Y 450

inline vector_t vector_add(vector_t a,vector_t b)
{
vector_t result;
result.x=a.x+b.x;
result.y=a.y+b.y;
return result;
}

inline vector_t vector_subtract(vector_t a,vector_t b)
{
vector_t result;
result.x=a.x-b.x;
result.y=a.y-b.y;
return result;
}

inline vector_t vector_multiply(double a,vector_t b)
{
vector_t result;
result.x=a*b.x;
result.y=a*b.y;
return result;
}

inline double vector_magnitude(vector_t a)
{
return sqrt(a.x*a.x+a.y*a.y);
}

inline vector_t vector_normalize(vector_t a)
{
return vector_multiply(1/vector_magnitude(a),a);
}

inline double vector_dot(vector_t a,vector_t b)
{
return a.x*b.x+a.y*b.y;
}

inline vector_t rotate_vector(vector_t a,double angle)
{
vector_t result;
result.x=cos(angle)*a.x+sin(angle)*a.y;
result.y=-sin(angle)*a.x+cos(angle)*a.y;
return result;
}

inline vector_t vector_transform(vector_t a,camera_t camera)
{
return vector_multiply(camera.scale,rotate_vector(vector_subtract(a,camera.position),camera.rotation));
}

void object_render(object_t* object,camera_t camera)
{
const color_t white=get_color(255,255,255);
int i;
vector_t prev_point;
vector_t cur_point;

prev_point=vector_transform(vector_add(object->position,rotate_vector(object->points[object->num_points-1],object->rotation)),camera);
	for(i=0;i<object->num_points;i++)
	{
	cur_point=vector_transform(vector_add(object->position,rotate_vector(object->points[i],object->rotation)),camera);
	draw_line((unsigned int)(prev_point.x+OFFSET_X),(unsigned int)(prev_point.y+OFFSET_Y),(unsigned int)(cur_point.x+OFFSET_X),(unsigned int)(cur_point.y+OFFSET_Y),white);
	prev_point=cur_point;
	}
}


void object_calculate_forces(object_t* objects,int num_objects)
{
int i,j;
//Calculate forces on objects
	for(i=0;i<num_objects;i++)
	for(j=i+1;j<num_objects;j++)
	{
	//Get distance and direction
	vector_t displacement=vector_subtract(objects[j].position,objects[i].position);
	double distance=vector_magnitude(displacement);
	vector_t direction=vector_multiply(1/distance,displacement);
	
	//Calculate gravity
	double gravity=GRAVITATIONAL_CONSTANT*(objects[i].mass*objects[j].mass)/(distance*distance);
	vector_t force=vector_multiply(gravity,direction);

	objects[i].force=vector_add(objects[i].force,force);
	objects[j].force=vector_subtract(objects[j].force,force);
	}
}

void object_init_physics(object_t* objects,int num_objects)
{
int i;
object_calculate_forces(objects,num_objects);
	for(i=0;i<num_objects;i++)
	{
	objects[i].acceleration=vector_multiply(1/objects[i].mass,objects[i].force);
	}
}
void player_physics(object_t* p);
void object_physics(object_t* objects,int num_objects,double delta_t)
{
int i;

double delta_t_2=delta_t*delta_t;

	for(i=0;i<num_objects;i++)
	{
	//Calculate V(t+1/2dt)
	objects[i].velocity.x+=0.5*objects[i].acceleration.x*delta_t;
	objects[i].velocity.y+=0.5*objects[i].acceleration.y*delta_t;
	//Calculate X(t+dt)
	objects[i].position.x+=objects[i].velocity.x*delta_t;
	objects[i].position.y+=objects[i].velocity.y*delta_t;
	//While we're iterating over objects, reset forces for next step
	objects[i].force.x=0;
	objects[i].force.y=0;
	}
player_physics(objects+11);
object_calculate_forces(objects,num_objects);
	
	for(i=0;i<num_objects;i++)
	{
	//Calculate A(t+dt)
	objects[i].acceleration=vector_multiply(1/objects[i].mass,objects[i].force);
	//Calculate V(t+dt);
	objects[i].velocity.x+=0.5*objects[i].acceleration.x*delta_t;
	objects[i].velocity.y+=0.5*objects[i].acceleration.y*delta_t;
	//Update rotation;
	objects[i].rotation+=objects[i].delta_rot*delta_t;
	}
}

object_set_satellite(object_t* object,object_t* centre,double angle,double radius)
{
object->position.x=centre->position.x+radius*sin(angle);
object->position.y=centre->position.y+radius*cos(angle);

double velocity=sqrt(GRAVITATIONAL_CONSTANT*centre->mass/radius);
object->velocity=centre->velocity;
object->velocity.x+=velocity*cos(angle);
object->velocity.y+=velocity*-sin(angle);
}


void asteroid_generate(object_t* asteroid,double mass,double radius)
{
int i;

asteroid->position.x=0;
asteroid->position.y=0;
asteroid->velocity.x=0;
asteroid->velocity.y=0;
asteroid->acceleration.x=0;
asteroid->acceleration.y=0;
asteroid->rotation=0;
asteroid->delta_rot=0;
asteroid->radius=radius;
asteroid->mass=mass;

uniform_dist_t radius_dist=create_uniform_dist(radius/2.0,radius*3.0/2.0);
uniform_dist_t angle_dist=create_uniform_dist(-0.5,0.5);

double angle=0;
	for(i=0;i<8;i++)
	{
	double perturbed_radius=rand_uniform(radius_dist);
	double perturbed_angle=angle+rand_uniform(radius_dist);
	asteroid->points[i].x=perturbed_radius*sin(angle);
	asteroid->points[i].y=perturbed_radius*cos(angle);
	angle+=M_PI/4;
	}

asteroid->num_points=8;
}

void planet_generate(object_t* planet,double mass,double radius)
{
int i;

planet->position.x=0;
planet->position.y=0;
planet->velocity.x=0;
planet->velocity.y=0;
planet->acceleration.x=0;
planet->acceleration.y=0;
planet->rotation=0;
planet->delta_rot=0;
planet->radius=radius;
planet->mass=mass;

uniform_dist_t radius_dist=create_uniform_dist(radius-5e3,radius+5e3);

double angle=0;
	for(i=0;i<32;i++)
	{
	double perturbed_radius=rand_uniform(radius_dist);
	planet->points[i].x=perturbed_radius*sin(angle);
	planet->points[i].y=perturbed_radius*cos(angle);
	angle+=M_PI/16;
	}

planet->num_points=32;
}

void asteroid_belt_generate(object_t* asteroids,int num_asteroids,normal_dist_t asteroid_radii,normal_dist_t orbital_radii,object_t* centre)
{
int i;
uniform_dist_t angle_dist=create_uniform_dist(0,2*M_PI);
	for(i=0;i<num_asteroids;i++)
	{
	double angle=rand_uniform(angle_dist);
	double radius=rand_normal(orbital_radii);
	asteroid_generate(&asteroids[i],1,rand_normal(asteroid_radii));
	object_set_satellite(&asteroids[i],centre,angle,radius);
	}
}


void player_generate(object_t* player)
{
player->points[0].x=-1e4;
player->points[0].y=1e4;
player->points[1].x=-1e4;
player->points[1].y=-1e4;
player->points[2].x=1e4;
player->points[2].y=-1e4;
player->points[3].x=1e4;
player->points[3].y=1e4;
player->points[4].x=0;
player->points[4].y=3e4;
player->num_points=5;
player->radius=0;
player->mass=1;
player->rotation=0;
player->delta_rot=0;
}

void player_physics(object_t* player)
{
	if(key_down(KEY_UP))
	{
	player->force.x=10*sin(player->rotation);
	player->force.y=10*cos(player->rotation);
	}
	
	if(key_down(KEY_LEFT))player->delta_rot=1;
	else if(key_down(KEY_RIGHT))player->delta_rot=-1;
	else player->delta_rot=0;
}


void create_solar_system(object_t* objects)
{
double AU=1.496e11;

//Create sun
planet_generate(objects,1.99e30,696e6);

//Create mercury
planet_generate(objects+1,3.30e23,244e4);
object_set_satellite(objects+1,objects,0,0.0395*AU);

//Create venus
planet_generate(objects+2,4.87e24,605e4);
object_set_satellite(objects+2,objects,1,0.723*AU);

//Create earth
planet_generate(objects+3,5.97e24,637e4);
object_set_satellite(objects+3,objects,2,AU);
//Create moon
planet_generate(objects+10,735e20,174e4);
object_set_satellite(objects+10,objects+3,0,385e6);

//Create mars
planet_generate(objects+4,1,337e4);
object_set_satellite(objects+4,objects,3,1.53*AU);

//Create jupiter
planet_generate(objects+5,1,692e5);
object_set_satellite(objects+5,objects,4,5.21*AU);

//Create saturn
planet_generate(objects+6,1,573e5);
object_set_satellite(objects+6,objects,5,9.55*AU);

//Create uranus
planet_generate(objects+7,1,253e5);
object_set_satellite(objects+7,objects,6,19.2*AU);

//Create neptune
planet_generate(objects+8,1,2e5);
object_set_satellite(objects+8,objects,7,30.1*AU);

//Create pluto
planet_generate(objects+9,1,117e4);
object_set_satellite(objects+9,objects,8,40.7*AU);

}


inline void draw_cross(vector_t centre,color_t color)
{
draw_line((unsigned int)(centre.x+OFFSET_X-20),(unsigned int)(centre.y+OFFSET_Y-20),(unsigned int)(centre.x+OFFSET_X+20),(unsigned int)(centre.y+OFFSET_Y+20),color);
draw_line((unsigned int)(centre.x+OFFSET_X+20),(unsigned int)(centre.y+OFFSET_Y-20),(unsigned int)(centre.x+OFFSET_X-20),(unsigned int)(centre.y+OFFSET_Y+20),color);
}

vector_t ellipse_get_point(vector_t focus,double semi_latus_rectum,double eccentricity,double argument_of_periapsis,double theta)
{
vector_t point;
double radius=semi_latus_rectum/(1+eccentricity*cos(theta-argument_of_periapsis));
point.x=focus.x+radius*cos(theta);
point.y=focus.y+radius*sin(theta);
return point;
}

void show_orbit(object_t* object,object_t* objects,int num_objects,camera_t camera)
{
int i;

double nearest_distance;
object_t* centre=NULL;
	for(i=0;i<num_objects;i++)
	{
		if(objects+i==object)continue;
	double distance=vector_magnitude(vector_subtract(objects[i].position,object->position));
		if(i==0)nearest_distance=distance;
		if(distance>nearest_distance)continue;
	double velocity=vector_magnitude(vector_subtract(object->velocity,objects[i].velocity));
	double escape_velocity=sqrt(2*GRAVITATIONAL_CONSTANT*objects[i].mass/distance);
		if(velocity<escape_velocity)
		{
		nearest_distance=distance;
		centre=objects+i;
		}
	}
	if(centre==NULL)return;

vector_t rel_position=vector_subtract(object->position,centre->position);
vector_t rel_velocity=vector_subtract(object->velocity,centre->velocity);
vector_t direction=vector_normalize(rel_position);

double k=GRAVITATIONAL_CONSTANT*centre->mass;
double specific_angular_momentum=rel_position.x*rel_velocity.y-rel_position.y*rel_velocity.x;

//This vector has the mass terms cancelled out, because I'm only interested in the shape, which doesn't depend on mass
vector_t laplace_runge_lenz;
laplace_runge_lenz.x=specific_angular_momentum*rel_velocity.y-k*direction.x;
laplace_runge_lenz.y=-specific_angular_momentum*rel_velocity.x-k*direction.y;

double A=vector_magnitude(laplace_runge_lenz);

double argument_of_periapsis=atan2(laplace_runge_lenz.y,laplace_runge_lenz.x);
double semi_latus_rectum=specific_angular_momentum*specific_angular_momentum/k;
double eccentricity=A/k;

//Plot orbit
const color_t green=get_color(0,255,0);
double theta=-M_PI;
vector_t prev_point=vector_transform(ellipse_get_point(centre->position,semi_latus_rectum,eccentricity,argument_of_periapsis,theta),camera);

	for(i=0;i<128;i++)
	{
	theta+=M_PI/64;
	vector_t cur_point=vector_transform(ellipse_get_point(centre->position,semi_latus_rectum,eccentricity,argument_of_periapsis,theta),camera);
	draw_line((unsigned int)(prev_point.x+OFFSET_X),(unsigned int)(prev_point.y+OFFSET_Y),(unsigned int)(cur_point.x+OFFSET_X),(unsigned int)(cur_point.y+OFFSET_Y),green);
	prev_point=cur_point;
	}
//Plot apoapsis and periapsis
const color_t red=get_color(255,0,0);
draw_cross(vector_transform(ellipse_get_point(centre->position,semi_latus_rectum,eccentricity,argument_of_periapsis,argument_of_periapsis),camera),red);
draw_cross(vector_transform(ellipse_get_point(centre->position,semi_latus_rectum,eccentricity,argument_of_periapsis,argument_of_periapsis+M_PI),camera),red);
}

void main_loop()
{
object_t objects[64];
double delta_t=0.02;

create_solar_system(objects);

camera_t camera;
camera.scale=1e-4;


player_generate(objects+11);
object_set_satellite(objects+11,objects+3,0,objects[3].radius+160e3);


//asteroid_belt_generate(objects+1,15,create_normal_dist(3,1),create_normal_dist(210,3),0,0,objects[0].mass);
object_init_physics(objects,10);
	while(!key_down(KEY_ESCAPE))
	{
	process_keys();
		if(key_pressed(KEY_MINUS))camera.scale*=0.5;
		else if(key_pressed(KEY_PLUS))camera.scale*=2;
	
		if(key_pressed(KEY_LESS)&&delta_t>1.1)delta_t*=0.1;
		else if(key_pressed(KEY_GREATER)&&delta_t<10001)delta_t*=10;
	camera.position=objects[11].position;
	camera.rotation=-objects[11].rotation-M_PI/2;
	clear_screen();

	//player_physics(objects+11,0.02);
	object_physics(objects,12,delta_t);

	show_orbit(objects+11,objects,12,camera);
	int i;
		for(i=0;i<12;i++)object_render(&objects[i],camera);
	usleep(20000);
	}
}
