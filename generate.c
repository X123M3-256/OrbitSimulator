#include<stdlib.h>
#include<math.h>
#include "generate.h"
#include "orbit.h"

/*
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

*/


double get_sphere_of_influence(celestial_body_t* body)
{
return body->orbit.semi_major_axis*pow(body->base.mass/body->orbit.primary->base.mass,0.4);
}


void star_generate(celestial_body_t* star,const char* name,double mass,double radius)
{
star->base.position.x=0;
star->base.position.y=0;
star->base.velocity.x=0;
star->base.velocity.y=0;
star->base.acceleration.x=0;
star->base.acceleration.y=0;
star->base.rotation=0;
star->base.delta_rot=0;
star->base.mass=mass;

star->name=name;
star->radius=radius;
//Star is not in orbit
star->sphere_of_influence=0;
star->orbit.primary=NULL;


//Generate geometry
int i;
double angle=0;
	for(i=0;i<32;i++)
	{
	star->base.points[i].x=radius*sin(angle);
	star->base.points[i].y=radius*cos(angle);
	angle+=M_PI/16;
	}
star->base.num_points=32;
}

void planet_generate(celestial_body_t* planet,const char* name,double mass,double radius,double angular_velocity,orbit_t orbit,double true_longitude)
{
planet->base.position=orbit_get_position(&orbit,true_longitude);
planet->base.velocity=orbit_get_velocity_vector(&orbit,true_longitude);
planet->base.acceleration.x=0;
planet->base.acceleration.y=0;
planet->base.rotation=0;
planet->base.delta_rot=angular_velocity;
planet->base.mass=mass;

planet->name=name;
planet->radius=radius;
planet->orbit=orbit;
planet->sphere_of_influence=get_sphere_of_influence(planet);


//Generate geometry
int i;
double angle=0;
	for(i=0;i<32;i++)
	{
	planet->base.points[i].x=radius*sin(angle);
	planet->base.points[i].y=radius*cos(angle);
	angle+=M_PI/16;
	}
planet->base.num_points=32;
}


void generate_solar_system(simulation_t* sim)
{
double AU=1.496e11;

//Gets angular velocity from period in days
#define PERIOD_DAYS(x) (2*M_PI/((x)*86400))

//Create sun
celestial_body_t* sun=simulation_new_body(sim);
star_generate(sun,"Sun",1.99e30,696e6);

//Create mercury
celestial_body_t* mercury=simulation_new_body(sim);
planet_generate(mercury,"Mercury",3.30e23,244e4,PERIOD_DAYS(58.646),orbit_create(sun,5.791e10,2.056e-1,1.352),0);

//Create venus
celestial_body_t* venus=simulation_new_body(sim);
planet_generate(venus,"Venus",4.87e24,605e4,PERIOD_DAYS(-243.02),orbit_create(sun,1.082e11,6.773e-3,2.296),0);

//Create earth
celestial_body_t* earth=simulation_new_body(sim);
planet_generate(earth,"Earth",5.97e24,6.37e6,PERIOD_DAYS(0.99727),orbit_create(sun,1.496e11,1.671e-2,1.795),0);

	//Create moon
	celestial_body_t* moon=simulation_new_body(sim);
	planet_generate(moon,"Moon",7.35e22,1.74e6,PERIOD_DAYS(27.322),orbit_create(earth,3.844e8,5.54e-2,0),0);

//Create mars
celestial_body_t* mars=simulation_new_body(sim);
planet_generate(mars,"Mars",6.42e23,3.37e6,PERIOD_DAYS(1.0260),orbit_create(sun,2.279e11,9.341e-2,5.865),-0.7816);

//Create jupiter
celestial_body_t* jupiter=simulation_new_body(sim);
planet_generate(jupiter,"Jupiter",1.90e27,6.92e7,PERIOD_DAYS(0.41353),orbit_create(sun,7.784e11,4.839e-2,0.256),0);

//Create saturn
celestial_body_t* saturn=simulation_new_body(sim);
planet_generate(saturn,"Saturn",5.68e26,5.73e7,PERIOD_DAYS(0.44403),orbit_create(sun,1.427e12,5.415e-2,1.613),0);

//Create uranus
celestial_body_t* uranus=simulation_new_body(sim);
planet_generate(uranus,"Uranus",8.68e25,2.53e7,PERIOD_DAYS(-0.71833),orbit_create(sun,2.871e12,4.717e-2,2.932),0);

//Create neptune
celestial_body_t* neptune=simulation_new_body(sim);
planet_generate(neptune,"Neptune",1.02e26,2e5,PERIOD_DAYS(0.67125),orbit_create(sun,4.498e12,8.586e-3,0.785),0);

//Create pluto
celestial_body_t* pluto=simulation_new_body(sim);
planet_generate(pluto,"Pluto",1.31e22,1.117e6,PERIOD_DAYS(-6.3872),orbit_create(sun,5.906e12,2.488e-1,3.911),0);

sim->primary=sun;
}

