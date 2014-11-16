#include <math.h>
#include "vector.h"

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
return vector_add(vector_multiply(camera.scale,rotate_vector(vector_subtract(a,camera.position),camera.rotation)),camera.offset);
}

