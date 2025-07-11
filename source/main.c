#include <stdio.h>

#include "../include/polygon.h"
#include "../include/screen.h"

int main(int argc, char const *argv[]) {
    const int height = 128;
    const int width = 512;

    if (init_window(height, width) != 0)
        return 1;

    vertex_t myv = new_vertex(0, 0, 10);
    plot(myv);
    draw_window();

    destroy_window();
    return 0;
}