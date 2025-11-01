#include <math.h>

#include "../include/mesh.h"

// normalize : normalize vector v
vertex_t normalize(vertex_t v) {
    double magnitude = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (magnitude == 0.0) 
        return new_vertex(0, 0, 0);
    return new_vertex(v.x / magnitude, v.y / magnitude, v.z / magnitude);
}

// dot : calculates the dot product of both vertices
float dot(vertex_t v0, vertex_t v1) {
    return (float) (v0.x * v1.x + v0.y * v1.y + v0.z * v1.z);
}

// cross : calculates the cross product of v0 and v1
vertex_t cross(vertex_t v0, vertex_t v1) {
    return new_vertex(
        (v0.y * v1.z) - (v0.z * v1.y),
        (v0.z * v1.x) - (v0.x * v1.z),
        (v0.x * v1.y) - (v0.y * v1.x)
    );
}

// vsubtract : subtracts v0 by v1 and returns the resulting vector
vertex_t vsubtract(vertex_t v0, vertex_t v1) {
    return new_vertex(
        v0.x - v1.x, 
        v0.y - v1.y, 
        v0.z - v1.z);
}