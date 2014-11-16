#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include "stats.h"

uniform_dist_t create_uniform_dist(float min,float max)
{
uniform_dist_t dist;
dist.min=min;
dist.max=max;
return dist;
}

normal_dist_t create_normal_dist(float mean,float std_dev)
{
normal_dist_t dist;
dist.mean=mean;
dist.variance=std_dev*std_dev;
return dist;
}

float rand_uniform(uniform_dist_t dist)
{
float value=(float)rand()/(float)RAND_MAX;
return dist.min+value*(dist.max-dist.min);
}

float rand_normal(normal_dist_t dist)
{
static int has_value=0;
static float value=0;

	if(has_value)
	{
	has_value=0;
	return dist.mean+dist.variance*value;
	}

const uniform_dist_t uniform_dist={-1.0,1.0};
float x,y,s;
	do
	{
	x=rand_uniform(uniform_dist);
	y=rand_uniform(uniform_dist);
	}while((s=(x*x+y*y))>=1||s==0.0);

float mult=sqrt(-2*log(s)/s);
has_value=1;
value=x*mult;
return dist.mean+dist.variance*y*mult;
}


