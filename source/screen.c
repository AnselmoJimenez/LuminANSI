#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <math.h>
#include <float.h>

#include "../include/object.h"
#include "../include/screen.h"

struct termios original_state;

// disable_raw_mode : restore original terminal settings
static void disable_raw_mode(void) { 
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_state); 
}

// enable_raw_mode : sets the terminal into raw mode
static void enable_raw_mode(void) {
    tcgetattr(STDIN_FILENO, &original_state);   // Current terminal settings
    atexit(disable_raw_mode);   // Register cleanup function to restore settings on exit

    struct termios new_state = original_state;
    new_state.c_lflag &= ~(ICANON | ECHO | ISIG);  // Disable canonical mode, echo, and signals
    new_state.c_cc[VMIN]  = 0;  // Minimum characters to return - Don't wait
    new_state.c_cc[VTIME] = 0;  // No timeout

    // Apply the new settings
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_state);
}

int init = 0; // initialization flag

typedef struct {
    int height;
    int width;
    char **pixels;
    float *zbuffer;
} window_t;

window_t window;
float focal_length;

#define CHARLIMIT   4
#define PATTERNSIZE 13
const char *pattern[PATTERNSIZE] = {
    " ",             // empty space
    "\xe2\xa1\x80",  // ⡀
    "\xe2\xa3\x80",  // ⣀
    "\xe2\xa3\x84",  // ⣄
    "\xe2\xa3\xa4",  // ⣤
    "\xe2\xa3\xa6",  // ⣦
    "\xe2\xa3\xb6",  // ⣶
    "\xe2\xa3\xb7",  // ⣷
    "\xe2\xa3\xbf",  // ⣿
    "\xe2\x96\x91",  // ░ (light shade)
    "\xe2\x96\x92",  // ▒ (medium shade)
    "\xe2\x96\x93",  // ▓ (dark shade)
    "\xe2\x96\x88"   // █ (full block)
};

#define CURSOR_HOME     "\x1B\x5B\x48"
#define CLEAR_SCREEN    "\x1B\x5B\x32\x4A"
#define HIDE_CURSOR     "\x1B\x5B\x3F\x32\x35\x6C"
#define SHOW_CURSOR     "\x1B\x5B\x3F\x32\x35\x68"

// init_window : initialize a window of size height and width
int init_window(const int height, const int width) {
    enable_raw_mode();

    // initialize screen variables
    window.height = height;
    window.width = width;
    window.pixels = (char **) malloc((window.height * window.width) * sizeof(char *));
    window.zbuffer = (float *) malloc((window.height * window.width) * sizeof(float));
    if (window.pixels == NULL) return 1;
    
    // initialize the screen and zbuffer
    for (int i = 0; i < window.height * window.width; i++) {
        window.pixels[i] = (char *) malloc(CHARLIMIT * sizeof(char));  // Space + null terminator
        strcpy(window.pixels[i], pattern[0]);
        window.zbuffer[i] = FLT_MAX;
    }

    focal_length = window.width / 2;

    printf(CLEAR_SCREEN);
    printf(HIDE_CURSOR);
    init = 1;

    return 0;
}

// destroy_window: free the memory contained in the window
void destroy_window(void) {
    if (!init) return;

    // free the pixels and zbuffer
    for (int i = 0; i < window.height * window.width; i++)
        free(window.pixels[i]);
    free(window.pixels);
    free(window.zbuffer);

    disable_raw_mode();
    printf(SHOW_CURSOR);
}

// keypress : returns keypress or -1 if no key was pressed
int keypress(void) {
    char c;
    if (read(STDIN_FILENO, &c, 1) == 1) 
        return c;
    return -1;
}

// draw_window : draw the buffer of characters
void draw_window(void) {
    for (int i = 0; i < window.height * window.width; i++) {
        if (i % window.width == 0 && i != 0) printf("\n");
        printf("%s", window.pixels[i]);
    }
    printf("\n");
}

// clear_window : reset the window pixels
void clear_window(void) {
    for (int i = 0; i < window.height * window.width; i++) {
        strcpy(window.pixels[i], pattern[0]);
        window.zbuffer[i] = FLT_MAX;
    }
    printf(CURSOR_HOME);
}

// normalize : normalize vector v
static vertex_t normalize(vertex_t v) {
    double magnitude = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (magnitude == 0.0) 
        return new_vertex(0, 0, 0);
    return new_vertex(v.x / magnitude, v.y / magnitude, v.z / magnitude);
}

// dot : calculates the dot product of both vertices
static float dot(vertex_t v0, vertex_t v1) {
    return (float) (v0.x * v1.x + v0.y * v1.y + v0.z * v1.z);
}

// cross : calculates the cross product of v0 and v1
static vertex_t cross(vertex_t v0, vertex_t v1) {
    return new_vertex(
        (v0.y * v1.z) - (v0.z * v1.y),
        (v0.z * v1.x) - (v0.x * v1.z),
        (v0.x * v1.y) - (v0.y * v1.x)
    );
}

// vsubtract : subtracts v0 by v1 and returns the resulting vector
static vertex_t vsubtract(vertex_t v0, vertex_t v1) {
    return new_vertex(
        v0.x - v1.x, 
        v0.y - v1.y, 
        v0.z - v1.z);
}

#define CAMERA_Z -8

#define MAX(a, b) ((a) > (b)) ? (a) : (b)
#define MIN(a, b) ((a) < (b)) ? (a) : (b)

typedef struct pixel {
    int x;     // screen pixel x coordinate
    int y;     // screen pixel y coordinate
    float z;   // 3D space z coordinate
} pixel_t;

static pixel_t new_pixel(int x, int y, float z) {
    return (pixel_t) {
        .x = x,
        .y = y,
        .z = z
    };
}

// wtopx : converts 3D world coordinates to pixel coordinates
static pixel_t wtopx(vertex_t v) {
    return (pixel_t) {
        .x = (int) ((window.width / 2)  + (v.x * focal_length) / (v.z - CAMERA_Z)),
        .y = (int) ((window.height / 2) - (v.y * focal_length) / (v.z - CAMERA_Z)),
        .z = (float) v.z
    };
}

// plotpx : Plot pixel in pixels buffer
static void plotpx(int x, int y, float z, const char *c) {
    // array index
    int index = (window.width * y) + x;
    if (index > -1 && index < window.height * window.width) {
        if (z <= window.zbuffer[index]) { 
            window.zbuffer[index] = z;
            strcpy(window.pixels[index], c);
        }
    }
}

// swap : swap values of the indexes a and b
static void swap(pixel_t *a, pixel_t *b) {
    pixel_t temp = *a;
    *a = *b;
    *b = temp;
}

typedef struct intersection {
    pixel_t left, right;
} intersection_t;

// fill_surface: fills in the surface while interpolating z values
static void fill_surface(intersection_t intersections, int y, float intensity) {
    // light intensity mapping
    int index = (int) MAX(0, MIN(PATTERNSIZE - 1, intensity * (PATTERNSIZE - 1)));
    const char *c = pattern[intensity < 0 ? 0 : index];  // Handle backfaces

    // calculate deltas
    int dx = intersections.right.x - intersections.left.x;
    float dz = intersections.right.z - intersections.left.z;
    
    // Interpolate through z
    for (int x = intersections.left.x; x < intersections.right.x; x++) {
        float t = (float) (x - intersections.left.x) / dx;
        float z = intersections.left.z + t * dz;

        plotpx(x, y, z, c);
    }
}

// get_intersection: Obtains the intersection at a scanline given the parameter edges
static intersection_t get_intersection(pixel_t endpoints[3], int y) {
    intersection_t result;
    pixel_t hits[2] = {0};
    unsigned int count = 0;

    // Check each of the 3 edges
    for (unsigned int i = 0; i < 3; i++) {
        pixel_t px_start = endpoints[i];
        pixel_t px_end = endpoints[(i + 1) % 3];

        int min_y = (px_start.y < px_end.y) ? px_start.y : px_end.y;
        int max_y = (px_start.y > px_end.y) ? px_start.y : px_end.y;

        // if y does not exist within this edge, continue
        if (y < min_y || y >= max_y) continue;

        // Calculate intersection
        int dx = px_end.x - px_start.x;
        int dy = px_end.y - px_start.y;
        float dz = px_end.z - px_start.z;

        float t = (float)(y - px_start.y) / dy;
        int x = px_start.x + t * dx;
        float z = px_start.z + t * dz;

        // add this pixel to the hit array
        hits[count++] = new_pixel(x, y, z);
    }

    // sort by x values
    if (count == 2 && hits[0].x > hits[1].x)
        swap(&hits[0], &hits[1]);
    
    result.left = hits[0];
    result.right = (count == 2) ? hits[1] : hits[0];

    return result;
}

// get_brightness : calculates the brightness of a surface
static float get_brightness(vertex_t v0, vertex_t v1, vertex_t v2) {
    // calculate lighting
    vertex_t light = new_vertex(0.5, 1, -0.5);
    vertex_t light_direction = normalize(light);
    vertex_t surface_normal  = normalize(cross(vsubtract(v1, v0), vsubtract(v2, v0)));

    float ambient_light = 0.1f; // minimum brightness
    float diffuse = MAX(0, dot(surface_normal, light_direction));
    return ambient_light + (1.0f - ambient_light) * diffuse;
}

// draw_surface : draws the surface based on the face definition
void draw_surface(vertex_t v0, vertex_t v1, vertex_t v2) {
    float intensity = get_brightness(v0, v1, v2);

    // Initialize vertex screen coordinates
    pixel_t px0 = wtopx(v0);
    pixel_t px1 = wtopx(v1);
    pixel_t px2 = wtopx(v2);
    pixel_t endpoints[3] = { px0, px1, px2 };

    // Find MAX and MIN y value for the current surface
    int min_y = MIN(px0.y, MIN(px1.y, px2.y));
    int max_y = MAX(px0.y, MAX(px1.y, px2.y));

    // scan line, find intersections, and fill based on z coordinate
    for (int y = min_y; y <= max_y; y++) {
        intersection_t intersect = get_intersection(endpoints, y);

        // then fill scanline with pixel coordinates
        fill_surface(intersect, y, intensity);
    }
}