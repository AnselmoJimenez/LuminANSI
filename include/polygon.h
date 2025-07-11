typedef struct {
    double x;
    double y;
    double z;
    char *pattern;
} vertex_t;

// new_vertex : create and return a new vertex object
vertex_t new_vertex(double, double, double);

// rotate_x : rotate the vertex coordinate by angle in the x direction
void rotate_x(double, vertex_t *);

// rotate_y : rotate the vertex coordinate by angle in the y direction
void rotate_y(double, vertex_t *);

// rotate_z : rotate the vertex coordinate by angle in the z direction
void rotate_z(double, vertex_t *);