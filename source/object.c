#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#include "../include/object.h"
#include "../include/screen.h"

// new_vertex : create and return a new vertex object
vertex_t new_vertex(double x, double y, double z) { 
    return (vertex_t) { x, y, z };
}

// new_surface : Create and return a new surface object
surface_t new_surface(vertex_t v0, vertex_t v1, vertex_t v2) {
    return (surface_t) { v0, v1, v2 };
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

#define MAXSTRINGLEN 512    
int vcount = 0;
int vncount = 0;
int fcount = 0;

// load : loads the object file name
object_t load(FILE *fp) {
    object_t obj;
    char line[MAXSTRINGLEN] = { '\0' };

    while (fgets(line, MAXSTRINGLEN, fp) != NULL) {
        double x, y, z;
        switch (line[0]) {
            case 'v':   // vertex data
                switch (line[1]) {
                    case 'n':   // vertex normals
                    case 't':   // vertex textures
                        break;
                    default:    // vertex coordinates
                        if (vcount >= MAXVERTICES) break;
                        sscanf(line, "v %lf %lf %lf\n", &x, &y, &z);
                        obj.vertices[vcount++] = new_vertex(x, y, z);
                        break;
                }
                break;
            case 'f':   // face definition
                char *lp = &line[2];
                int subdivrootv = 0;
                sscanf(lp, "%d/%*d/%*d", &subdivrootv); // vertex subdivision origin
                for ( ; *lp != ' ' && *lp != '\n'; lp++)
                    ;
                if (*lp == ' ') lp++;

                int last_vertex;
                sscanf(lp, "%d/%*d/%*d", &last_vertex);
                for ( ; *lp != ' ' && *lp != '\n'; lp++)
                    ; // skip to next vertex and begin parsing

                if (*lp == ' ') lp++;

                while (*lp != '\n' && *lp != '\0' && *lp != EOF) {
                    face_t new_face;
                    new_face.vertex_index[0] = subdivrootv;
                    new_face.vertex_index[1] = last_vertex;
                    sscanf(lp, "%d/%*d/%*d", &new_face.vertex_index[2]);

                    for ( ; *lp != ' ' && *lp != '\n'; lp++)
                        ;
                    
                    if (*lp == ' ') lp++;
                    last_vertex = new_face.vertex_index[2];

                    if (fcount < MAXVERTICES * 2) {
                        obj.faces[fcount] = new_face;
                        ++fcount;
                    }
                }
                break;
            default: break;
        }
    }

    return obj;
}