#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/polygon.h"
#include "../include/screen.h"

typedef struct {
    int height;
    int width;
    char **pixels;
} window_t;

window_t window;

#define PATTERNSIZE 4
const char *pattern[] = {
    " ",             // empty space
    "\xe2\xa1\x80",  // ⡀
    "\xe2\xa3\x80",  // ⣀
    "\xe2\xa3\x84",  // ⣄
    "\xe2\xa3\xa4",  // ⣤
    "\xe2\xa3\xa6",  // ⣦
    "\xe2\xa3\xb6",  // ⣶
    "\xe2\xa3\xb7",  // ⣷
    "\xe2\xa3\xbf",  // ⣿
    "\xe2\x96\x88"   // █
};


// init_window : initialize a window of size height and width
int init_window(const int height, const int width) {
    window.height = height;
    window.width = width;
    window.pixels = malloc((window.height * window.width) * sizeof(char *));
    if (window.pixels == NULL)
        return 1;
    
    for (int i = 0; i < window.height * window.width; i++) {
        window.pixels[i] = malloc(PATTERNSIZE * sizeof(char));  // Space + null terminator
        strcpy(window.pixels[i], pattern[8]);
    }

    return 0;
}

// draw_window : draw the buffer of characters
int draw_window(void) {
    for (int i = 0; i < window.height * window.width; i++) {
        if (i % window.width == 0) printf("\n%s", window.pixels[i]);
        else printf("%s", window.pixels[i]);
    }
    printf("\n");
}

// plot : plot a pixel onto the window
void plot(vertex_t v) {
    int x_offset = v.x % window.width;
    int y_offset = v.y % window.height;
    window.pixels[y_offset * x_offset] = v.pattern;
}