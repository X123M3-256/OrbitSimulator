#ifndef RANDOM_INCLUDED
#define RANDOM_INCLUDED


typedef struct
{
float min,max;
}uniform_dist_t;

typedef struct
{
float mean,variance;
}normal_dist_t;

uniform_dist_t create_uniform_dist(float min,float max);
normal_dist_t create_normal_dist(float mean,float std_dev);
float rand_uniform(uniform_dist_t);
float rand_normal(normal_dist_t);


#endif
