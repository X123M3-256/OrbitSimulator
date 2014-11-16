#include <math.h>
#include "orbit.h"
#include "vector.h"
#include "draw.h"


orbit_t orbit_create(celestial_body_t* primary,double semi_major_axis,double eccentricity,double longitude_of_periapsis)
{
orbit_t orbit;
orbit.primary=primary;
orbit.semi_major_axis=semi_major_axis;
orbit.eccentricity=eccentricity;
orbit.longitude_of_periapsis=longitude_of_periapsis;
orbit.semi_latus_rectum=semi_major_axis*(1-eccentricity*eccentricity);
return orbit;
}

orbit_t orbit_calculate(celestial_body_t* primary,vector_t rel_position,vector_t rel_velocity)
{
vector_t direction=vector_normalize(rel_position);

double k=GRAVITATIONAL_CONSTANT*primary->base.mass;
double specific_angular_momentum=rel_position.x*rel_velocity.y-rel_position.y*rel_velocity.x;

//This vector has the mass terms cancelled out, because I'm only interested in the shape, which doesn't depend on mass
vector_t laplace_runge_lenz;
laplace_runge_lenz.x=specific_angular_momentum*rel_velocity.y-k*direction.x;
laplace_runge_lenz.y=-specific_angular_momentum*rel_velocity.x-k*direction.y;
double A=vector_magnitude(laplace_runge_lenz);

orbit_t orbit;
orbit.primary=primary;
orbit.longitude_of_periapsis=atan2(laplace_runge_lenz.y,laplace_runge_lenz.x);
orbit.semi_latus_rectum=specific_angular_momentum*specific_angular_momentum/k;
orbit.eccentricity=A/k;
orbit.semi_major_axis=orbit.semi_latus_rectum/(1-orbit.eccentricity*orbit.eccentricity);
return orbit;
}


double orbit_get_period(orbit_t* orbit)
{
return 2*M_PI*sqrt(orbit->semi_major_axis*orbit->semi_major_axis*orbit->semi_major_axis/(GRAVITATIONAL_CONSTANT*orbit->primary->base.mass));
}

double orbit_get_radius(orbit_t* orbit,double theta)
{
return orbit->semi_latus_rectum/(1+orbit->eccentricity*cos(theta-orbit->longitude_of_periapsis));
}

vector_t orbit_get_position(orbit_t* orbit,double theta)
{
vector_t point;
double radius=orbit_get_radius(orbit,theta);
point.x=orbit->primary->base.position.x+radius*cos(theta);
point.y=orbit->primary->base.position.y+radius*sin(theta);
return point;
}

double orbit_get_velocity_scalar(orbit_t* orbit,double theta)
{
return sqrt((GRAVITATIONAL_CONSTANT*orbit->primary->base.mass)*(2/orbit_get_radius(orbit,theta)-1/orbit->semi_major_axis));
}


vector_t orbit_get_velocity_vector(orbit_t* orbit,double theta)
{
vector_t velocity;
//Calculate radius and its derivative
double radius=orbit_get_radius(orbit,theta);
double radius_derivative=(orbit->eccentricity*sin(theta-orbit->longitude_of_periapsis)*radius*radius)/orbit->semi_latus_rectum;
//Calculate direction of motion
velocity.x=-radius_derivative*cos(theta)+radius*sin(theta);
velocity.y=-radius_derivative*sin(theta)-radius*cos(theta);
velocity=vector_normalize(velocity);
//Calculate velocity
double velocity_magnitude=sqrt((GRAVITATIONAL_CONSTANT*orbit->primary->base.mass)*(2/radius-1/orbit->semi_major_axis));
return vector_add(orbit->primary->base.velocity,vector_multiply(orbit_get_velocity_scalar(orbit,theta),velocity));
}

vector_t orbit_show(orbit_t* orbit,camera_t camera,color_t color)
{
//Plot orbit
double theta=-M_PI;
vector_t prev_point=vector_transform(orbit_get_position(orbit,theta),camera);

int i;
	for(i=0;i<128;i++)
	{
	theta+=M_PI/64;
	vector_t cur_point=vector_transform(orbit_get_position(orbit,theta),camera);
	draw_line((unsigned int)prev_point.x,(unsigned int)prev_point.y,(unsigned int)cur_point.x,(unsigned int)cur_point.y,color);
	prev_point=cur_point;
	}

}

