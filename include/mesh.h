#ifndef OBJECT_H
#define OBJECT_H

typedef struct _IO_FILE FILE;

typedef struct vertex{
    float x;
    float y;
    float z;
} vertex_t;

typedef struct surface {
    vertex_t v0;
    vertex_t v1;
    vertex_t v2;
} surface_t;

// new_vertex : create and return a new vertex object
vertex_t new_vertex(float x, float y, float z);

// roll : rotate the vertex coordinate by angle in the x direction about c
void roll(float angle, surface_t *surface);

// pitch : rotate the vertex coordinate by angle in the y direction about c
void pitch(float angle, surface_t *surface);

// yaw : rotate the vertex coordinate by angle in the z direction about c
void yaw(float angle, surface_t *surface);

#define MAXVERTICES     4096
#define EULERSPOLYHEDRA (2 * MAXVERTICES)

typedef struct mesh {
    int vcount;
    int fcount;
    vertex_t vertices[MAXVERTICES];
    surface_t surfaces[EULERSPOLYHEDRA];
} mesh_t;

// load : loads the object file name
mesh_t load(const char *filename, FILE *fp);

#endif