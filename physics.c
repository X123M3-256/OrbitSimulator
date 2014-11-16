#include <math.h>
#include "physics.h"
#include "sim.h"
#include "vector.h"



void object_step_position(object_t* object,double delta_t)
{
//Calculate V(t+1/2dt)
object->velocity.x+=0.5*object->acceleration.x*delta_t;
object->velocity.y+=0.5*object->acceleration.y*delta_t;
//Calculate X(t+dt)
object->position.x+=object->velocity.x*delta_t;
object->position.y+=object->velocity.y*delta_t;
//While we're iterating objects, reset forces for next step
object->force.x=0;
object->force.y=0;
}

void object_step_velocity(object_t* object,double delta_t)
{
//Calculate A(t+dt)
object->acceleration=vector_multiply(1/object->mass,object->force);
//Calculate V(t+dt);
object->velocity.x+=0.5*object->acceleration.x*delta_t;
object->velocity.y+=0.5*object->acceleration.y*delta_t;
//Update rotation;
object->rotation+=object->delta_rot*delta_t;
}

vector_t calculate_gravitation(object_t* a,object_t* b)
{
//Get distance and direction
vector_t displacement=vector_subtract(a->position,b->position);
double distance=vector_magnitude(displacement);
vector_t direction=vector_multiply(1/distance,displacement);

//Calculate gravity
double gravity=GRAVITATIONAL_CONSTANT*(a->mass*b->mass)/(distance*distance);

return vector_multiply(gravity,direction);
}


void calculate_forces(simulation_t* sim)
{
int i,j;
	for(i=0;i<sim->num_celestial_bodies;i++)
	{
		//Calculate forces between celestial_bodies
		for(j=i+1;j<sim->num_celestial_bodies;j++)
		{
		vector_t force=calculate_gravitation((object_t*)(&sim->celestial_bodies[j]),(object_t*)(&sim->celestial_bodies[i]));
		sim->celestial_bodies[i].base.force=vector_add(sim->celestial_bodies[i].base.force,force);
		sim->celestial_bodies[j].base.force=vector_subtract(sim->celestial_bodies[j].base.force,force);
		}

	}
}

void init_physics(simulation_t* sim)
{
int i;
calculate_forces(sim);
	for(i=0;i<sim->num_celestial_bodies;i++)sim->celestial_bodies[i].base.acceleration=vector_multiply(1/sim->celestial_bodies[i].base.mass,sim->celestial_bodies[i].base.force);
	for(i=0;i<sim->num_spacecraft;i++)sim->spacecraft[i].base.acceleration=vector_multiply(1/sim->spacecraft[i].base.mass,sim->spacecraft[i].base.force);
}

void spacecraft_collision(simulation_t* sim,spacecraft_t* spacecraft)
{
vector_t displacement=vector_subtract(spacecraft->base.position,spacecraft->orbit.primary->base.position);
double distance=vector_magnitude(displacement);
double penetration=spacecraft->orbit.primary->radius-distance;
    if(penetration>0)
    {
    displacement=vector_multiply((distance+penetration)/distance,displacement);
    spacecraft->base.position=vector_add(spacecraft->orbit.primary->base.position,displacement);
    spacecraft->base.velocity=spacecraft->orbit.primary->base.velocity;
    spacecraft->base.velocity.x+=spacecraft->orbit.primary->base.delta_rot*displacement.y;
    spacecraft->base.velocity.y+=-spacecraft->orbit.primary->base.delta_rot*displacement.x;
    spacecraft->base.rotation=atan2(-displacement.y,displacement.x)+M_PI/2;
    spacecraft->base.delta_rot=0;
    }
}



void spacecraft_position_docked(spacecraft_t* spacecraft)
{
int i;
    for(i=0;i<spacecraft->num_docks;i++)
    {
    dock_t* dock=spacecraft->docks+i;
        if(dock->docked_spacecraft!=NULL)
        {
        dock_t* docked_dock=&dock->docked_spacecraft->docks[dock->docked_dock];

        vector_t dock_1_position=rotate_vector(dock->position,dock->orientation);
        vector_t dock_2_position=rotate_vector(docked_dock->position,docked_dock->orientation);
        dock->docked_spacecraft->base.position=vector_add(spacecraft->base.position,rotate_vector(vector_add(dock_1_position,dock_2_position),spacecraft->base.rotation));
        dock->docked_spacecraft->base.rotation=spacecraft->base.rotation+dock->orientation+docked_dock->orientation+M_PI;
        }
    }
}



void spacecraft_run_physics(simulation_t* sim,spacecraft_t* spacecraft,double delta_t)
{
int i,j;

object_step_position((object_t*)spacecraft,delta_t);


spacecraft->base.force.x=0;
spacecraft->base.force.y=0;

    //Calculate forces between celestial bodies and spacecraft
    for(i=0;i<sim->num_celestial_bodies;i++)
    {
        for(j=0;j<sim->num_spacecraft;j++)
        {
        vector_t force=calculate_gravitation((object_t*)(sim->celestial_bodies+i),(object_t*)(sim->spacecraft+j));
        sim->spacecraft[j].base.force=vector_add(sim->spacecraft[j].base.force,force);
        }
    }

    //Calculate forces from rocket
    if(spacecraft->firing)
    {
    spacecraft->fuel-=delta_t*spacecraft->thrust/spacecraft->specific_impulse;
    spacecraft->base.mass=spacecraft->dry_mass+spacecraft->fuel;
    spacecraft->base.force.x+=spacecraft->thrust*sin(spacecraft->base.rotation);
    spacecraft->base.force.y+=spacecraft->thrust*cos(spacecraft->base.rotation);
        if(spacecraft->fuel<0)
        {
        spacecraft->firing=0;
        spacecraft->fuel=0;
        }
    }

object_step_velocity((object_t*)spacecraft,delta_t);

spacecraft_calculate_orbit(sim,spacecraft);
spacecraft_collision(sim,spacecraft);
//spacecraft_position_docked(spacecraft);
}

void run_physics(simulation_t* sim)
{
int i,j;
double delta_t;
int iterations=1;

double max_delta_t=sim->current_spacecraft->firing?MAX_THRUSTING_DELTA_T:MAX_DELTA_T;
delta_t=DELTA_T*sim->speedup;
	if(delta_t>max_delta_t)
	{
	delta_t=max_delta_t;
	iterations=sim->speedup*(DELTA_T/max_delta_t);
	}

	while(iterations--)
    {
    //This bit might be removed
        for(i=0;i<sim->num_celestial_bodies;i++)object_step_position((object_t*)(sim->celestial_bodies+i),delta_t);
        calculate_forces(sim);
        for(i=0;i<sim->num_celestial_bodies;i++)object_step_velocity((object_t*)(sim->celestial_bodies+i),delta_t);


        for(i=0;i<sim->num_spacecraft;i++)
        {
        spacecraft_t* spacecraft=sim->spacecraft+i;
        spacecraft_run_physics(sim,spacecraft,delta_t);
        }
    }
}

