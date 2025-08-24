typedef struct vertex{
    double x;
    double y;
    double z;
} vertex_t;

// new_vertex : create and return a new vertex object
vertex_t new_vertex(double x, double y, double z);

// rotate_x : rotate the vertex coordinate by angle in the x direction
void rotate_x(double angle, vertex_t *v);

// rotate_y : rotate the vertex coordinate by angle in the y direction
void rotate_y(double angle, vertex_t *v);

// rotate_z : rotate the vertex coordinate by angle in the z direction
void rotate_z(double angle, vertex_t *v);

#define FACEVERTICES 3

typedef struct face {
    int vertex_index[FACEVERTICES];
    int normal_index;
} face_t;

#define MAXVERTICES 1024

typedef struct object {
    vertex_t vertices[MAXVERTICES];
    vertex_t normals[MAXVERTICES];
    face_t faces[MAXVERTICES * 2];
} object_t;

extern int vcount;
extern int vncount;
extern int fcount;

// load : loads the object file name
object_t load(FILE *fp);