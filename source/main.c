#include <stdio.h>

#include "../include/polygon.h"
#include "../include/screen.h"

#define MAXVERTICES 50

vertex_t vertices[MAXVERTICES] = {0};
vertex_t *vp = vertices;

int main(int argc, char const *argv[]) {
    const int height = 64;
    const int width = 256;

    if (init_window(height, width) != 0)
        return 1;

    vertex_t center = new_vertex(0, 0, 11);
    *vp++ = new_vertex( 1,  1, 10);
    *vp++ = new_vertex(-1, -1, 10);
    *vp++ = new_vertex( 1, -1, 10);
    *vp++ = new_vertex(-1,  1, 10);
    *vp++ = new_vertex( 1,  1, 12);
    *vp++ = new_vertex(-1, -1, 12);
    *vp++ = new_vertex( 1, -1, 12);
    *vp++ = new_vertex(-1,  1, 12);
    
    plot(vertices[0]);
    plot(vertices[1]);

    connect(vertices[0], vertices[1]);
    draw_window();

    destroy_window();
    return 0;
}