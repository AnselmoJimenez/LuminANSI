#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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

double focal_length;

// init_window : initialize a window of size height and width
int init_window(const int height, const int width) {
    window.height = height;
    window.width = width;
    window.pixels = malloc((window.height * window.width) * sizeof(char *));
    if (window.pixels == NULL)
        return 1;
    
    for (int i = 0; i < window.height * window.width; i++) {
        window.pixels[i] = malloc(PATTERNSIZE * sizeof(char));  // Space + null terminator
        strcpy(window.pixels[i], pattern[0]);
    }

    focal_length = window.width / 2;

    return 0;
}

// destroy_window: free the memory contained in the window
void destroy_window(void) {
    for (int i = 0; i < window.height * window.width; i++)
        free(window.pixels[i]);
    free(window.pixels);
}

#define HIDE_CURSOR     "\x1B\x5B\x3F\x32\x35\x6C"
#define SHOW_CURSOR     "\x1B\x5B\x3F\x32\x35\x68"

// draw_window : draw the buffer of characters
void draw_window(void) {
    printf(HIDE_CURSOR);
    for (int i = 0; i < window.height * window.width; i++) {
        if (i % window.width == 0 && i != 0) printf("\n");
        printf("%s", window.pixels[i]);
    }
    printf("\n");
    printf(SHOW_CURSOR);
}

#define CURSOR_HOME     "\x1B\x5B\x48"
#define CLEAR_SCREEN    "\x1B\x5B\x32\x4A"

// clear_window : reset the window pixels
void clear_window(void) {
    for (int i = 0; i < window.height * window.width; i++)
        strcpy(window.pixels[i], pattern[0]);
    printf(CURSOR_HOME);
    printf(CLEAR_SCREEN);
}

// plot : put vertex in pixels buffer
void plot(vertex_t v) {
    // projected coordinated on a the image plane
    double projx = (v.x * focal_length) / v.z;
    double projy = (v.y * focal_length) / v.z;

    // convert to pixel coordinates
    double pxx = (window.width / 2) + projx;
    double pxy = (window.height / 2) + projy;

    // array index
    int index = (window.width * (int) pxy) + (int) pxx;
    if (index > -1 && index < window.height * window.width) // quick bounds check
        strcpy(window.pixels[index], v.pattern);
}

// connect : draw a line between vertices (Bresenham's Line Algorithm)
void connect(vertex_t v0, vertex_t v1) {
    // projected coordinated on a the image plane
    double projx0 = (v0.x * focal_length) / v0.z;
    double projy0 = (v0.y * focal_length) / v0.z;
    double projx1 = (v1.x * focal_length) / v1.z;
    double projy1 = (v1.y * focal_length) / v1.z;

    // convert to pixel coordinates
    double pxx0 = (window.width / 2) + projx0;
    double pxy0 = (window.height / 2) + projy0;
    double pxx1 = (window.width / 2) + projx1;
    double pxy1 = (window.height / 2) + projy1;

    // calculate differences
    double dx = abs(pxx1 - pxx0);
    double dy = abs(pxx1 - pxy0);

    // Determine direction
    int sx = (pxx0 < pxx1) ? 1 : -1;
    int sy = (pxy0 < pxy1) ? 1 : -1;

    double err = dx - dy;
    double x = pxx0, y = pxy0;

    while (1) {
        // Set pixel at (x, y)
        if (x > -1 && x < window.width && y > -1 && y < window.height) {
            int index = (window.width * y) + x;
            strcpy(window.pixels[index], pattern[8]);
        }
        
        if (x == (int) pxx1 && y == (int) pxy1) 
            break;
        
        double e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x += sx; }
        if (e2 < dx)  { err += dx; y += sy; }
    }
}