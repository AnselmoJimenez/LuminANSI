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

    *vp++ = new_vertex( 1,  1, 10);
    *vp++ = new_vertex(-1, -1, 10);
    *vp++ = new_vertex( 1, -1, 10);
    *vp++ = new_vertex(-1,  1, 10);
    for (int i = 0; i < 4; i++)
        plot(vertices[i]);
    draw_window();

    for (int timer = 0; timer < 50; timer++) {
        clear_window();
        for (int i = 0; i < 4; i++) {
            rotate_z(1, &vertices[i]);
            plot(vertices[i]);
        }
        draw_window();
    }

    destroy_window();
    return 0;
}