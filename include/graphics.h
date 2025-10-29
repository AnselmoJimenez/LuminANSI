#ifndef GRAPHICS_H
#define GRAPHICS_H

#define STRINGSIZE  20
#define PATTERNSIZE 15

typedef struct pixel {
    int x;     // screen pixel x coordinate
    int y;     // screen pixel y coordinate
    float z;   // 3D space z coordinate
    char c[STRINGSIZE];
} pixel_t;

typedef struct screen {
    int initialization; // Flag for initialization
    int height;         // height of the screen
    int width;          // width of the screen
    float focal_length; // focal length of the camera
    pixel_t *pixels;     // pixels of the screen
} screen_t;

typedef struct surface surface_t;

// draw_surface : draws the surface based on the face definition
void draw_surface(screen_t *window, surface_t surface);

#endif