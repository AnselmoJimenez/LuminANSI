#include <stdio.h>

#include "../include/polygon.h"
#include "../include/screen.h"

#define MAXVERTICES 50

vertex_t vertices[MAXVERTICES];
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
    // *vp++ = new_vertex( 1,  1, 12);
    // *vp++ = new_vertex(-1, -1, 12);
    // *vp++ = new_vertex( 1, -1, 12);
    // *vp++ = new_vertex(-1,  1, 12);
    
    for (int i = 0; i < 4; i++)
        plot(vertices[i]);

    for (int count = 0; count < 2000; count++) {
        clear_window();
        for (int i = 0; i < 4; i++) {
            rotate_y(0.0025, &vertices[i], center);
        }
        connect(vertices[1], vertices[2]);
        connect(vertices[2], vertices[3]);
        connect(vertices[3], vertices[0]);
        connect(vertices[0], vertices[2]);
        connect(vertices[1], vertices[3]);
        draw_window();
    }

    destroy_window();
    return 0;
}