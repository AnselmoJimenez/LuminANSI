#include <stdio.h>
#include <math.h>

#include "../include/polygon.h"

// new_vertex : create and return a new vertex object
vertex_t new_vertex(double x, double y, double z) {
    vertex_t v = {
        .x = x,
        .y = y,
        .z = z,
        .pattern = "\xe2\x96\x88"   // █
    };
    return v;
}

// rotate_x : rotate the vertex coordinate by angle in the x direction
void rotate_x(double angle, vertex_t *v) {
    double y1 = v->y;
    double z1 = v->z;
    v->y = y1 * cos(angle) - z1 * sin(angle);
    v->z = y1 * sin(angle) + z1 * cos(angle);
}

// rotate_y : rotate the vertex coordinate by angle in the y direction
void rotate_y(double angle, vertex_t *v) {
    double x1 = v->x;
    double z1 = v->z;
    v->x = x1 * cos(angle) + z1 * sin(angle);
    v->z = -x1 * sin(angle) + z1 * cos(angle);
}

// rotate_z : rotate the vertex coordinate by angle in the z direction
void rotate_z(double angle, vertex_t *v) {
    double x1 = v->x;
    double y1 = v->y;
    v->x = x1 * cos(angle) - y1 * sin(angle);
    v->y = x1 * sin(angle) + y1 * cos(angle);
}