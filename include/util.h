#ifndef UTIL_H
#define UTIL_H

#define INFINITE 2147483647

typedef struct vertex vertex_t;

// normalize : normalize vector v
vertex_t normalize(vertex_t v);

// dot : calculates the dot product of both vertices
float dot(vertex_t v0, vertex_t v1);

// cross : calculates the cross product of v0 and v1
vertex_t cross(vertex_t v0, vertex_t v1);

// vsubtract : subtracts v0 by v1 and returns the resulting vector
vertex_t vsubtract(vertex_t v0, vertex_t v1);

#endif