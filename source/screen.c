#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/screen.h"

typedef struct {
    int height;
    int width;
    char **pixels;
} window_t;
window_t window;

// init_window : initialize a window of size height and width
int init_window(const int height, const int width) {
    window.height = height;
    window.width = width;
    window.pixels = malloc((window.height * window.width) * sizeof(char *));
    if (window.pixels == NULL)
        return 1;
    
    char **pxb = window.pixels;
    while (window.pixels + (window.height * window.width) > pxb)
        *pxb++ = " ";
}