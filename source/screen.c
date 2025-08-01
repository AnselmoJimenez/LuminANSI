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

// connect : draw a line between vertices (DDA Line Algorithm)
void connect(vertex_t v0, vertex_t v1) {
    // convert both vertexes into pixel coordinates
    int pxx0 = (window.width / 2) + (v0.x * focal_length) / v0.z;
    int pxy0 = (window.height / 2) + (v0.y * focal_length) / v0.z;
    int pxx1 = (window.width / 2) + (v1.x * focal_length) / v1.z;
    int pxy1 = (window.height / 2) + (v1.y * focal_length) / v1.z;
    printf("(pxx0, pxy0) = (%d, %d) - (pxx1, pxy1) = (%d, %d)\n", pxx0, pxy0, pxx1, pxy1);

    int dx = pxx1 - pxx0;
    int dy = pxy1 - pxy0;

    // steps required for generating pixels
    int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);

    // calculate increment in x and y for each steps
    float xincrement = dx / (float) steps;
    float yincrement = dy / (float) steps;

    // put pixels for each step
    float x = pxx0;
    float y = pxy0;
    for (int i = 0; i <= steps; i++) {
        int index = (window.width * y) + x;
        if (index > -1 && index < window.height * window.width && strcmp(window.pixels[index], pattern[9]) != 0) // quick bounds check
            strcpy(window.pixels[index], pattern[8]);
        
        x += xincrement;
        y += yincrement;
    }
}