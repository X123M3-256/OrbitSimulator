#include<stdlib.h>
#include<stdio.h>
#include<math.h>

#include "sim.h"
#include "draw.h"
#include "keys.h"
#include "vector.h"
#include "physics.h"
#include "orbit.h"
#include "generate.h"





void draw_cross(vector_t centre,color_t color)
{
draw_line((unsigned int)(centre.x-10),(unsigned int)(centre.y-10),(unsigned int)(centre.x+10),(unsigned int)(centre.y+10),color);
draw_line((unsigned int)(centre.x+10),(unsigned int)(centre.y-10),(unsigned int)(centre.x-10),(unsigned int)(centre.y+10),color);
}

void draw_arrow(vector_t centre,double angle,color_t color)
{
//Define shape
vector_t tip;
tip.x=0;
tip.y=10;
vector_t left;
left.x=-5;
left.y=5;
vector_t right;
right.x=5;
right.y=5;
//Rotate arrow
tip=vector_add(centre,rotate_vector(tip,angle));
left=vector_add(centre,rotate_vector(left,angle));
right=vector_add(centre,rotate_vector(right,angle));

draw_line((unsigned int)centre.x,(unsigned int)centre.y,(unsigned int)tip.x,(unsigned int)tip.y,color);
draw_line((unsigned int)tip.x,(unsigned int)tip.y,(unsigned int)left.x,(unsigned int)left.y,color);
draw_line((unsigned int)tip.x,(unsigned int)tip.y,(unsigned int)right.x,(unsigned int)right.y,color);
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
	draw_line((unsigned int)prev_point.x,(unsigned int)prev_point.y,(unsigned int)cur_point.x,(unsigned int)cur_point.y,white);
	prev_point=cur_point;
	}
}



void spacecraft_calculate_orbit(simulation_t* sim,spacecraft_t* spacecraft)
{
//Determine smallest s.o.i that the spacecraft lies within
celestial_body_t* primary=sim->primary;
double smallest_soi=vector_magnitude(vector_subtract(sim->primary->base.position,spacecraft->base.position));
int i;
	for(i=0;i<sim->num_celestial_bodies;i++)
	{
	celestial_body_t* body=&sim->celestial_bodies[i];
		if(body->orbit.primary!=NULL&&body->sphere_of_influence<smallest_soi&&body->sphere_of_influence>vector_magnitude(vector_subtract(body->base.position,spacecraft->base.position)))
		{
		primary=body;
		smallest_soi=body->sphere_of_influence;
		}
	}

//Calculate spacecraft orbit
spacecraft->orbit=orbit_calculate(primary,vector_subtract(spacecraft->base.position,primary->base.position),vector_subtract(spacecraft->base.velocity,primary->base.velocity));
}


void get_si_prefix(double value,double* out,char* prefix)
{
double magnitude=fabs(value);
	if(magnitude>1e9)
	{
	*out=value*1e-9;
	*prefix='G';
	}
	else if(magnitude>1e6)
	{
	*out=value*1e-6;
	*prefix='M';
	}
	else if(magnitude>1e3)
	{
	*out=value*1e-3;
	*prefix='k';
	}
	else
	{
	*out=value;
	*prefix=' ';
	}
}

void get_time_as_non_si(double time,int* years,int* days,int* hours,int* minutes)
{
*years=(int)floor(time/31557600);
time=fmod(time,31557600);
*days=(int)floor(time/86400);
time=fmod(time,86400);
*hours=(int)floor(time/3600);
time=fmod(time,3600);
*minutes=(int)floor(time/60);
}

//Internally, angles are measured clockwise from the initial line in radians.
//This converts such an angle to a counter clockwise measurement in degrees
double format_angle(double angle)
{
return (angle>0?2*M_PI-angle:-angle)*180/M_PI;
}


void render_orbit_info(orbit_t* orbit,double theta,int x,int y)
{
char str[64];
double value,agl_value;
char si_prefix,agl_prefix;

double altitude=orbit_get_radius(orbit,theta);
double velocity=orbit_get_velocity_scalar(orbit,theta);
double escape_velocity=sqrt(2*GRAVITATIONAL_CONSTANT*orbit->primary->base.mass/altitude);


//Show orbital velocity
get_si_prefix(velocity,&value,&si_prefix);
sprintf(str,"Orbital Velocity      : %.2f%cm/s",value,si_prefix);
draw_text(x,y,str);
y+=10;

//Show escape velocity
get_si_prefix(escape_velocity,&value,&si_prefix);
sprintf(str,"Escape Velocity       : %.2f%cm/s",value,si_prefix);
draw_text(x,y,str);
y+=10;

//If orbit is elliptical, show period and apoapsis
	if(orbit->eccentricity<1)
	{
	//Show period
	int years,days,hours,minutes;
	get_time_as_non_si(orbit_get_period(orbit),&years,&days,&hours,&minutes);
	sprintf(str,"Orbital period        : %d years %d days %d hours %d mins",years,days,hours,minutes);
	draw_text(x,y,str);
	y+=10;

	//Show apoapsis
	double apoapsis_radius=orbit_get_radius(orbit,orbit->longitude_of_periapsis+M_PI);
	double agl=apoapsis_radius-orbit->primary->radius;
	get_si_prefix(apoapsis_radius,&value,&si_prefix);
	get_si_prefix(agl,&agl_value,&agl_prefix);
	sprintf(str,"Apoapsis              : %.2f%cm (%.2f%cm AGL)",value,si_prefix,agl_value,agl_prefix);
	draw_text(x,y,str);
	y+=10;
	}
	else
	{
	double excess_velocity=sqrt(-GRAVITATIONAL_CONSTANT*orbit->primary->base.mass/orbit->semi_major_axis);
	get_si_prefix(excess_velocity,&value,&si_prefix);
	sprintf(str,"Hyperbolic excess velocity: %.2f%cm/s",value,si_prefix);
	draw_text(x,y,str);
	y+=10;
	}

//Show periapsis
double periapsis_radius=orbit_get_radius(orbit,orbit->longitude_of_periapsis);
double agl=periapsis_radius-orbit->primary->radius;
get_si_prefix(periapsis_radius,&value,&si_prefix);
get_si_prefix(agl,&agl_value,&agl_prefix);
sprintf(str,"Periapsis             : %.2f%cm (%.2f%cm AGL)",value,si_prefix,agl_value,agl_prefix);
draw_text(x,y,str);
y+=10;

//Show true longitude
sprintf(str,"True longitude        : %.2f degrees",format_angle(theta));
draw_text(x,y,str);
y+=10;

//Show semimajor axis
get_si_prefix(orbit->semi_major_axis,&value,&si_prefix);
sprintf(str,"Semimajor axis        : %.2f%cm",value,si_prefix);
draw_text(x,y,str);
y+=10;

//Show eccentricity
sprintf(str,"Eccentricity          : %.2f",orbit->eccentricity);
draw_text(x,y,str);
y+=10;

//Show longitude of periapsis
sprintf(str,"Longitude of periapsis: %.2f degrees",format_angle(orbit->longitude_of_periapsis));
draw_text(x,y,str);
}


void render_spacecraft_info(spacecraft_t* spacecraft,int x,int y)
{
char str[64];
double value;
char si_prefix=' ';
double agl_value;
char agl_prefix;


const color_t green=get_color(0,255,0);

const double fuel_bar_height=100;

int base=y+fuel_bar_height+10;
int top=base-(int)(fuel_bar_height*spacecraft->fuel/spacecraft->capacity);

//Draw fuel bar
draw_text(x+7,base+10,"Fuel");
draw_line(x+13,base,x+13,top,green);
draw_line(x+13,top,x+23,top,green);
draw_line(x+23,top,x+23,base,green);
draw_line(x+23,base,x+13,base,green);

sprintf(str,"%.0fkg",spacecraft->fuel);
draw_text(x,top-10,str);
x+=50;

sprintf(str,"Mass             : %.1fkg",spacecraft->base.mass);
draw_text(x,y,str);
y+=10;

get_si_prefix(spacecraft->firing?spacecraft->thrust/spacecraft->base.mass:0,&value,&si_prefix);
sprintf(str,"Acceleration     : %.1f%cm/s^2",value,si_prefix);
draw_text(x,y,str);
y+=10;


get_si_prefix(spacecraft->thrust,&value,&si_prefix);
sprintf(str,"Thrust           : %.1f%cN",value,si_prefix);
draw_text(x,y,str);
y+=10;

sprintf(str,"Specific impulse : %.1fkg",spacecraft->specific_impulse);
draw_text(x,y,str);
y+=10;
/*
double altitude=vector_magnitude(vector_subtract(spacecraft->orbit.primary->base.position,spacecraft->base.position));
double altitude_agl=altitude-spacecraft->orbit.primary->radius;
get_si_prefix(altitude,&value,&si_prefix);
get_si_prefix(altitude_agl,&agl_value,&agl_prefix);
sprintf(str,"Altitude         : %.2f%cm (%.2f%cm AGL)",value,si_prefix,agl_value,agl_prefix);
draw_text(x,y,str);
y+=10;
*/

//vector_t rel_position=vector_subtract(spacecraft->base.position,spacecraft->orbit.primary->base.position);
//render_orbit_info(&spacecraft->orbit,atan2(rel_position.y,rel_position.x),x,y);
}


void render_celestial_body_info(celestial_body_t* celestial_body,int x,int y)
{
char str[64];
double value;
char si_prefix;


draw_text(x,y,celestial_body->name);
y+=10;

sprintf(str,"Mass    : %.2ekg",celestial_body->base.mass);
draw_text(x,y,str);
y+=10;

get_si_prefix(celestial_body->radius,&value,&si_prefix);
sprintf(str,"Radius  : %.2f%cm",value,si_prefix);
draw_text(x,y,str);
y+=10;

	if(celestial_body->orbit.primary==NULL)return;
vector_t rel_position=vector_subtract(celestial_body->base.position,celestial_body->orbit.primary->base.position);
render_orbit_info(&celestial_body->orbit,atan2(rel_position.y,rel_position.x),x,y);
}


void create_simulation(simulation_t* sim)
{
sim->camera.scale=1e1;
sim->camera.offset.x=(double)draw_get_width()/2;
sim->camera.offset.y=(double)draw_get_height()/2;
sim->speedup=1;
sim->current_spacecraft=NULL;
sim->num_celestial_bodies=0;
sim->num_spacecraft=0;
}

celestial_body_t* simulation_new_body(simulation_t* sim)
{
return sim->celestial_bodies+(sim->num_celestial_bodies++);
}

spacecraft_t* simulation_new_spacecraft(simulation_t* sim)
{
return sim->spacecraft+(sim->num_spacecraft++);
}

void simulation_render(simulation_t* sim)
{
const color_t red=get_color(255,0,0);
const color_t white=get_color(255,255,255);

orbit_t* current_orbit=&sim->current_spacecraft->orbit;


//Calculate camera position and orientation
sim->camera.position=sim->current_spacecraft->base.position;
//vector_t primary_to_cam=vector_subtract(sim->camera.position,current_orbit->primary->base.position);
sim->camera.rotation=0;//atan2(primary_to_cam.y,primary_to_cam.x)+M_PI_2;



//Plot apoapsis and periapsis
vector_t apoapsis=vector_transform(orbit_get_position(current_orbit,current_orbit->longitude_of_periapsis+M_PI),sim->camera);
vector_t periapsis=vector_transform(orbit_get_position(current_orbit,current_orbit->longitude_of_periapsis),sim->camera);
draw_cross(apoapsis,red);
draw_cross(periapsis,red);
draw_text(apoapsis.x-20,apoapsis.y-20,"Apoapsis");
draw_text(periapsis.x-20,periapsis.y-20,"Periapsis");
//Render arrow on spacecraft
draw_arrow(vector_transform(sim->current_spacecraft->base.position,sim->camera),sim->current_spacecraft->base.rotation+sim->camera.rotation,get_color(0,255,255));
//Show orbit
orbit_show(current_orbit,sim->camera,get_color(0,255,0));

char str[64];
//Show speedup
	if(sim->speedup==1)draw_text(20,650,"Real time");
	else
	{
	sprintf(str,"Fast forward: %dx",sim->speedup);

	draw_text(20,650,str);
	}


int i;
	for(i=0;i<sim->num_celestial_bodies;i++)
	{
	//Show orbit
		if(sim->celestial_bodies[i].orbit.primary!=NULL)orbit_show(&sim->celestial_bodies[i].orbit,sim->camera,get_color(255,255,0));
	//Draw planet
	object_render((object_t*)(&sim->celestial_bodies[i]),sim->camera);
	//Show name
	vector_t position=vector_transform(sim->celestial_bodies[i].base.position,sim->camera);
	draw_text(position.x,position.y,sim->celestial_bodies[i].name);
	//Draw sphere of influence
	int j;
	double theta=0;
		for(j=0;j<32;j++)
		{
		vector_t line_start=sim->celestial_bodies[i].base.position;
		line_start.x+=sim->celestial_bodies[i].sphere_of_influence*sin(theta);
		line_start.y+=sim->celestial_bodies[i].sphere_of_influence*cos(theta);
		line_start=vector_transform(line_start,sim->camera);
		theta+=M_PI/32;
		vector_t line_end=sim->celestial_bodies[i].base.position;
		line_end.x+=sim->celestial_bodies[i].sphere_of_influence*sin(theta);
		line_end.y+=sim->celestial_bodies[i].sphere_of_influence*cos(theta);
		line_end=vector_transform(line_end,sim->camera);
		theta+=M_PI/32;
		draw_line(line_start.x,line_start.y,line_end.x,line_end.y,white);
		}
	}
	for(i=0;i<sim->num_spacecraft;i++)
	{
	object_render((object_t*)(&sim->spacecraft[i]),sim->camera);
	}
render_spacecraft_info(sim->current_spacecraft,50,50);
//render_celestial_body_info(sim->current_spacecraft->orbit.primary,500,50);
//render_celestial_body_info(&sim->celestial_bodies[5],950,50);
}


spacecraft_dock(spacecraft_t* a,int a_dock_index,spacecraft_t* b,int b_dock_index)
{
dock_t* a_dock=a->docks+a_dock_index;
dock_t* b_dock=b->docks+b_dock_index;
a_dock->docked_spacecraft=b;
a_dock->docked_dock=b_dock_index;
b_dock->docked_spacecraft=a;
b_dock->docked_dock=a_dock_index;
b->base.position=vector_add(a->base.position,vector_subtract(rotate_vector(a_dock->position,a_dock->orientation),rotate_vector(b_dock->position,b_dock->orientation)));
b->parent=a->parent==NULL?a:a->parent;
}

spacecraft_undock(spacecraft_t* spacecraft,int dock_index)
{
dock_t* dock=spacecraft->docks+dock_index;

//Get velocity
vector_t velocity;
    if(spacecraft->parent!=NULL)velocity=spacecraft->parent->base.velocity;
    else velocity=spacecraft->base.velocity;

velocity.x=0;
velocity.y=0;

//Set velocities
dock->docked_spacecraft->base.velocity=velocity;
spacecraft->base.velocity=velocity;

//Undock spacecraft
dock->docked_spacecraft->docks[dock->docked_dock].docked_spacecraft=NULL;
dock->docked_spacecraft=NULL;

//Update parents
spacecraft->parent=NULL;
}

void spacecraft_add_dock(spacecraft_t* spacecraft,vector_t position,double orientation)
{
spacecraft->docks[spacecraft->num_docks].docked_spacecraft=NULL;
spacecraft->docks[spacecraft->num_docks].position=position;
spacecraft->docks[spacecraft->num_docks].orientation=orientation;
spacecraft->num_docks++;
}

spacecraft_t* upper_stage_generate(simulation_t* sim)
{
spacecraft_t* spacecraft=simulation_new_spacecraft(sim);
spacecraft->base.points[0].x=-2;
spacecraft->base.points[0].y=2;
spacecraft->base.points[1].x=-2;
spacecraft->base.points[1].y=-2;
spacecraft->base.points[2].x=2;
spacecraft->base.points[2].y=-2;
spacecraft->base.points[3].x=2;
spacecraft->base.points[3].y=2;
spacecraft->base.points[4].x=0;
spacecraft->base.points[4].y=6;
spacecraft->base.num_points=5;

spacecraft->dry_mass=2247;
spacecraft->num_docks=0;
spacecraft->parent=NULL;

spacecraft->base.position=sim->celestial_bodies[3].base.position;
spacecraft->base.position.y+=sim->celestial_bodies[3].radius;
spacecraft->base.velocity=sim->celestial_bodies[3].base.velocity;
spacecraft->base.rotation=0;
spacecraft->base.delta_rot=0;


spacecraft->thrust=99200;
spacecraft->capacity=20830;
spacecraft->fuel=20830;
spacecraft->specific_impulse=4463.55;


vector_t dock;
dock.x=0;
dock.y=-2;
spacecraft_add_dock(spacecraft,dock,M_PI);


spacecraft->base.mass=spacecraft->dry_mass+spacecraft->fuel;
}

spacecraft_t* lower_stage_generate(simulation_t* sim)
{
spacecraft_t* spacecraft=simulation_new_spacecraft(sim);
double height=32.46;
double width=3.81;
spacecraft->base.points[0].x=-width/2;
spacecraft->base.points[0].y=height/2;
spacecraft->base.points[1].x=-width/2;
spacecraft->base.points[1].y=-height/2;
spacecraft->base.points[2].x=width/2;
spacecraft->base.points[2].y=-height/2;
spacecraft->base.points[3].x=width/2;
spacecraft->base.points[3].y=height/2;
spacecraft->base.num_points=4;

spacecraft->dry_mass=37597.8;
spacecraft->num_docks=0;
spacecraft->parent=NULL;

spacecraft->base.position=sim->celestial_bodies[3].base.position;
spacecraft->base.position.y+=sim->celestial_bodies[3].radius;
spacecraft->base.velocity=sim->celestial_bodies[3].base.velocity;
spacecraft->base.rotation=0;
spacecraft->base.delta_rot=0;


spacecraft->thrust=4152e3;
spacecraft->capacity=344411.8;
spacecraft->fuel=344411.8;
spacecraft->specific_impulse=3.05e3;

vector_t dock;
dock.x=0;
dock.y=height/2;

spacecraft_add_dock(spacecraft,dock,0);

spacecraft->base.mass=spacecraft->dry_mass+spacecraft->fuel;
}

//Temp method to generate simple spacecraft
void spacecraft_generate(simulation_t* sim)
{
spacecraft_t* upper_stage=upper_stage_generate(sim);
spacecraft_t* lower_stage=lower_stage_generate(sim);
spacecraft_dock(lower_stage,0,upper_stage,0);
sim->current_spacecraft=lower_stage;
}





void simulation_process_input(simulation_t* sim)
{
	if(key_pressed(KEY_MINUS))sim->camera.scale*=0.5;
	else if(key_pressed(KEY_PLUS))sim->camera.scale*=2;
	if(key_pressed(KEY_LEFT_BRACKET)&&sim->speedup>1)sim->speedup/=10;
	else if(key_pressed(KEY_RIGHT_BRACKET))sim->speedup*=10;

sim->current_spacecraft->firing=key_down(KEY_UP)&&sim->current_spacecraft->fuel>0;

    if(key_down(KEY_LEFT))sim->current_spacecraft->base.delta_rot+=0.1/sim->speedup;
    else if(key_down(KEY_RIGHT))sim->current_spacecraft->base.delta_rot-=0.1/sim->speedup;

    //Temporary, for testing docking
    if(key_pressed(KEY_DOWN))
    {
    spacecraft_t* craft=sim->current_spacecraft->docks[0].docked_spacecraft;
    printf("%d\n",craft->parent);
    spacecraft_undock(sim->current_spacecraft,0);
    //sim->current_spacecraft=craft;
    int i;
        for(i=0;i<sim->num_spacecraft;i++)sim->spacecraft[i].parent=NULL;
    }
}
