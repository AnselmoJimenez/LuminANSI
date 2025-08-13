#include <stdio.h>
#include <math.h>

#include "../include/object.h"
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
void rotate_x(double angle, vertex_t *v) {
    double y1 = v->y;
    double z1 = v->z;
    v->y = y1 * cos(angle) - z1 * sin(angle);
    v->z = y1 * sin(angle) + z1 * cos(angle);
}

// rotate_y : rotate the vertex coordinate by angle in the y direction about c
void rotate_y(double angle, vertex_t *v) {
    double x1 = v->x;
    double z1 = v->z;
    v->x = x1 * cos(angle) + z1 * sin(angle);
    v->z = -x1 * sin(angle) + z1 * cos(angle);
}

// rotate_z : rotate the vertex coordinate by angle in the z direction about c
void rotate_z(double angle, vertex_t *v) {
    double x1 = v->x;
    double y1 = v->y;
    v->x = x1 * cos(angle) - y1 * sin(angle);
    v->y = x1 * sin(angle) + y1 * cos(angle);
}

object_t obj;

// load : loads the object file name
void loadobj(char *filename) {

}