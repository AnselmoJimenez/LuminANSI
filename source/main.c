#include <stdio.h>

#include "../include/polygon.h"
#include "../include/screen.h"

int main(int argc, char const *argv[]) {
    const int height = 15; // 128
    const int width = 30;  // 512

    if (init_window(height, width) != 0)
        return 1;

    draw_window();

    vertex_t myv = {
        .x = 15,
        .y = 5,
        .z = 0,
        .pattern = "\xe2\x96\x88"   // █
    };

    plot(myv);
    draw_window();
    return 0;
}