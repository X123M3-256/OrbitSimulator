#ifndef VECTOR_INCLUDED
#define VECTOR_INCLUDED
#include "sim.h"

inline vector_t vector_add(vector_t a,vector_t b);
inline vector_t vector_subtract(vector_t a,vector_t b);
inline vector_t vector_multiply(double a,vector_t b);
inline double vector_magnitude(vector_t a);
inline vector_t vector_normalize(vector_t a);
inline double vector_dot(vector_t a,vector_t b);
inline vector_t rotate_vector(vector_t a,double angle);
inline vector_t vector_transform(vector_t a,camera_t camera);

#endif
