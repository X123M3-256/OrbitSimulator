#ifndef ORBIT_INCLUDED
#define ORBIT_INCLUDED
#include "sim.h"


orbit_t orbit_create(celestial_body_t* primary,double semi_major_axis,double eccentricity,double longitude_of_periapsis);
orbit_t orbit_calculate(celestial_body_t* primary,vector_t position,vector_t velocity);
double orbit_get_period(orbit_t* orbit);
double orbit_get_radius(orbit_t* orbit,double theta);
vector_t orbit_get_position(orbit_t* orbit,double theta);
double orbit_get_velocity_scalar(orbit_t* orbit,double theta);
vector_t orbit_get_velocity_vector(orbit_t* orbit,double theta);
vector_t orbit_show(orbit_t* orbit,camera_t camera,color_t color);

#endif
