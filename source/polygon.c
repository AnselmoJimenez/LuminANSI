#include <stdio.h>
#include <math.h>

#include "../include/polygon.h"
#include "../include/screen.h"

// new_vertex : create and return a new vertex object
vertex_t new_vertex(double x, double y, double z) {
    vertex_t v = {
        .x = x,
        .y = y,
        .z = z,
    };

    return v;
}

// rotate_x : rotate the vertex coordinate by angle in the x direction about c
void rotate_x(double angle, vertex_t *v, vertex_t c) {
    double y1 = v->y - c.y;
    double z1 = v->z - c.z;
    v->y = y1 * cos(angle) - z1 * sin(angle) + c.y;
    v->z = y1 * sin(angle) + z1 * cos(angle) + c.z;
}

// rotate_y : rotate the vertex coordinate by angle in the y direction about c
void rotate_y(double angle, vertex_t *v, vertex_t c) {
    double x1 = v->x - c.x;
    double z1 = v->z - c.z;
    v->x = x1 * cos(angle) + z1 * sin(angle) + c.x;
    v->z = -x1 * sin(angle) + z1 * cos(angle) + c.z;
}

// rotate_z : rotate the vertex coordinate by angle in the z direction about c
void rotate_z(double angle, vertex_t *v, vertex_t c) {
    double x1 = v->x - c.x;
    double y1 = v->y - c.y;
    v->x = x1 * cos(angle) - y1 * sin(angle) + c.x;
    v->y = x1 * sin(angle) + y1 * cos(angle) + c.y;
}