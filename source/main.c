#include <stdio.h>

#include "../include/polygon.h"
#include "../include/screen.h"

#define MAXVERTICES 50

vertex_t vertices[MAXVERTICES];
vertex_t transforms[MAXVERTICES];

int main(int argc, char const *argv[]) {
    const int height = 64;
    const int width = 256;
    double rotation_angle = 0;

    if (init_window(height, width) != 0)
        return 1;

    vertex_t center = new_vertex(0, 0, 11);
    vertices[0] = new_vertex( 1,  1, 10);
    vertices[1] = new_vertex(-1, -1, 10);
    vertices[2] = new_vertex( 1, -1, 10);
    vertices[3] = new_vertex(-1,  1, 10);
    vertices[4] = new_vertex( 1,  1, 12);
    vertices[5] = new_vertex(-1, -1, 12);
    vertices[6] = new_vertex( 1, -1, 12);
    vertices[7] = new_vertex(-1,  1, 12);

    for (int count = 0; count < 4000; count++) {
        clear_window();

        rotation_angle += 0.005;

        for (int i = 0; i < 8; i++) {
            transforms[i] = vertices[i];
            rotate_y(rotation_angle, &transforms[i], center);
            rotate_x(rotation_angle, &transforms[i], center);
            plot(transforms[i]);
        }
        connect(transforms[0], transforms[2]);
        connect(transforms[0], transforms[3]);
        connect(transforms[0], transforms[4]);
        connect(transforms[1], transforms[2]);
        connect(transforms[1], transforms[3]);
        connect(transforms[1], transforms[5]);
        connect(transforms[2], transforms[6]);
        connect(transforms[3], transforms[7]);
        connect(transforms[4], transforms[6]);
        connect(transforms[4], transforms[7]);
        connect(transforms[5], transforms[6]);
        connect(transforms[5], transforms[7]);

        draw_window();
    }
    destroy_window();
    return 0;
}