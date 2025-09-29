#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <math.h>

#include "../include/object.h"
#include "../include/screen.h"

struct termios original_state;

// disable_raw_mode : restore original terminal settings
void disable_raw_mode(void) { 
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_state); 
}

// enable_raw_mode : sets the terminal into raw mode
void enable_raw_mode(void) {
    tcgetattr(STDIN_FILENO, &original_state);   // Current terminal settings
    atexit(disable_raw_mode);   // Register cleanup function to restore settings on exit

    struct termios new_state = original_state;
    new_state.c_lflag &= ~(ICANON | ECHO | ISIG);  // Disable canonical mode, echo, and signals
    new_state.c_cc[VMIN]  = 0;  // Minimum characters to return - Don't wait
    new_state.c_cc[VTIME] = 0;  // No timeout

    // Apply the new settings
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_state);
}

// flag if initialization has occurred
int init = 0;

typedef struct {
    int height;
    int width;
    char **pixels;
    int *zbuffer;
} window_t;

window_t window;
vertex_t light;
double focal_length;

#define CHARLIMIT   4
#define PATTERNSIZE 13
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
    "\xe2\x96\x91",  // ░ (light shade)
    "\xe2\x96\x92",  // ▒ (medium shade)
    "\xe2\x96\x93",  // ▓ (dark shade)
    "\xe2\x96\x88"   // █ (full block)
};

#define INFINITE        2147483647
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
    window.zbuffer = (int *) malloc((window.height * window.width) * sizeof(int));
    if (window.pixels == NULL) return 1;
    
    // initialize the screen and zbuffer
    for (int i = 0; i < window.height * window.width; i++) {
        window.pixels[i] = malloc(CHARLIMIT * sizeof(char));  // Space + null terminator
        strcpy(window.pixels[i], pattern[0]);
        window.zbuffer[i] = INFINITE;
    }

    focal_length = window.width / 2;
    light = new_vertex(10, 10, -10);

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
        window.zbuffer[i] = INFINITE;
    }
    printf(CURSOR_HOME);
}

// intncpy : copy n integer values from src to dest
static void intncpy(unsigned int *dest, unsigned int *src, unsigned int n) {
    if (dest == NULL || src == NULL) return;
    for (; n > 0; --n) *dest++ = *src++;
}

// swap : swap values of the indexes a and b
static void swap(unsigned int *a, unsigned int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// quicksort : sorts the elements of an unsigned integer array
static void quicksort(unsigned int *arr, unsigned int low, unsigned int high) {
    if (low >= high) return;

    // begin partitioning
    int pivot = arr[high];
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (arr[j] <= pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    ++i;

    quicksort(arr, low, i - 1);
    quicksort(arr, i + 1, high);
}

// normalize : normalize vector v
static vertex_t normalize(vertex_t v) {
    double magnitude = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (magnitude == 0.0) 
        return new_vertex(0, 0, 0);
    return new_vertex(v.x / magnitude, v.y / magnitude, v.z / magnitude);
}

// dot : calculates the dot product of both vertices
static double dot(vertex_t v0, vertex_t v1) {
    return v0.x * v1.x + v0.y * v1.y + v0.z * v1.z;
}

#define CAMERA_Z -5

#define MAX(a, b) ((a) > (b)) ? (a) : (b)
#define MIN(a, b) ((a) < (b)) ? (a) : (b)

typedef struct pixel {
    int x;
    int y;
} pixel_t;

static pixel_t new_pixel(int x, int y) {
    return (pixel_t) {
        .x = x,
        .y = y
    };
}

// wtopx : converts 3D world coordinates to pixel coordinates
static pixel_t wtopx(vertex_t v) {
    return (pixel_t) {
        .x = (int) ((window.width / 2)  + (v.x * focal_length) / (v.z - CAMERA_Z)),
        .y = (int) ((window.height / 2) - (v.y * focal_length) / (v.z - CAMERA_Z))
    };
}

// plotpx : Plot pixel in pixels buffer
static void plotpx(int x, int y, int z) {
    // array index
    int index = (window.width * y) + x;
    if (index > -1 && index < window.height * window.width) {
        if (z < window.zbuffer[index]) { 
            window.zbuffer[index] = z;
            strcpy(window.pixels[index], pattern[12]);
        }
    }
}

typedef struct edge {
    pixel_t *pixels;
    int *zbuffer;
    unsigned int length;
} edge_t;

// bresenham : Draw a line between vertices
static edge_t get_edge(vertex_t v0, vertex_t v1) {
    edge_t edge;
    int maxlength = (window.width > window.height) ? window.width : window.height;
    edge.pixels = (pixel_t *) malloc(maxlength * sizeof(pixel_t));
    edge.zbuffer = (int *) malloc(maxlength * sizeof(int));
    edge.length = 0;

    // convert both vertexes into pixel coordinates
    pixel_t px0 = wtopx(v0);
    pixel_t px1 = wtopx(v1);

    // calculating deltas
    int dx = abs(px1.x - px0.x);
    int dy = abs(px1.y - px0.y);
    int dz = abs(v1.z - v0.z);

    // Determining the direction of the line
    int xs = (px0.x < px1.x) ? 1 : -1;
    int ys = (px0.y < px1.y) ? 1 : -1;
    int zs = (v0.z  < v1.z ) ? 1 : -1;

    int x = (int) px0.x;
    int y = (int) px0.y;
    double z = v0.z;
    
    if (dx >= dy && dx >= dz) { // X is the driving axis
        int p0 = 2 * dy - dx;
        int p1 = (int) (2 * dz - dx);
        for (int i = 0; i < dx + 1; i++) {
            edge.pixels[edge.length]  = new_pixel(x, y);
            edge.zbuffer[edge.length] = (int) z;
            ++edge.length;
            if (p0 >= 0) {
                y += ys;
                p0 -= 2 * dx;
            }
            if (p1 >= 0) {
                z += zs;
                p1 -= 2 * dx;
            }
            p0 += 2 * dy;
            p1 += 2 * dz;
            x += xs;
        }
    }
    else if (dy >= dx && dy >= dz) { // Y is the driving axis
        int p0 = 2 * dx - dy;
        int p1 = (int) (2 * dz - dy);
        for (int i = 0; i < dy + 1; i++) {
            edge.pixels[edge.length]  = new_pixel(x, y);
            edge.zbuffer[edge.length] = (int) z;
            ++edge.length;
            if (p0 >= 0) {
                x += xs;
                p0 -= 2 * dy;
            }
            if (p1 >= 0) {
                z += zs;
                p1 -= 2 * dy;
            }
            p0 += 2 * dx;
            p1 += 2 * dz;
            y += ys;
        }
    }
    else if (dz >= dx && dz >= dy) { // Z is the driving axis
        int p0 = (int) (2 * dy - dz);
        int p1 = (int) (2 * dx - dz);
        for (int i = 0; i < dz + 1; i++) {
            edge.pixels[edge.length]  = new_pixel(x, y);
            edge.zbuffer[edge.length] = (int) z;
            ++edge.length;
            if (p0 >= 0) {
                y += ys;
                p0 -= 2 * dz;
            }
            if (p1 >= 0) {
                x += xs;
                p1 -= 2 * dz;
            }
            p0 += 2 * dy;
            p1 += 2 * dx;
            z += zs;
        }
    }

    edge.pixels  = (pixel_t *) realloc(edge.pixels, edge.length * sizeof(pixel_t));
    edge.zbuffer = (int *) realloc(edge.zbuffer, edge.length * sizeof(int));
    return edge;
}

// draw_surface : draws the surface based on the face definition
void draw_surface(vertex_t v0, vertex_t v1, vertex_t v2, vertex_t vn) {
    edge_t e0 = get_edge(v0, v1);
    edge_t e1 = get_edge(v1, v2);
    edge_t e2 = get_edge(v0, v2);

    for (int i = 0; i < e0.length; i++) plotpx(e0.pixels[i].x, e0.pixels[i].y, e0.zbuffer[i]);
    for (int i = 0; i < e1.length; i++) plotpx(e1.pixels[i].x, e1.pixels[i].y, e1.zbuffer[i]);
    for (int i = 0; i < e2.length; i++) plotpx(e2.pixels[i].x, e2.pixels[i].y, e2.zbuffer[i]);

    free(e0.pixels); free(e0.zbuffer); e0.pixels = NULL; e0.zbuffer = NULL;
    free(e1.pixels); free(e1.zbuffer); e1.pixels = NULL; e1.zbuffer = NULL;
    free(e2.pixels); free(e2.zbuffer); e2.pixels = NULL; e2.zbuffer = NULL;
}