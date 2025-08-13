typedef struct {
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

#define MAXVERTICES 1000

typedef struct object {
    char *name;
    vertex_t vertices[MAXVERTICES];
    float vnormals[MAXVERTICES];
    int fdefinitions[MAXVERTICES];
} object_t;

extern object_t obj;

// load : loads the object file name
void load(char *filename);