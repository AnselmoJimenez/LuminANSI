#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <math.h>

#include "../include/object.h"
#include "../include/screen.h"

// flag if initialization has occurred
int init = 0;

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

struct termios original_state;

// disable_raw_mode : restore original terminal settings
void disable_raw_mode(void) { tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_state); }

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

double focal_length;

#define CURSOR_HOME     "\x1B\x5B\x48"
#define CLEAR_SCREEN    "\x1B\x5B\x32\x4A"
#define HIDE_CURSOR     "\x1B\x5B\x3F\x32\x35\x6C"
#define SHOW_CURSOR     "\x1B\x5B\x3F\x32\x35\x68"

// init_window : initialize a window of size height and width
int init_window(const int height, const int width) {
    enable_raw_mode();

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
    printf(CLEAR_SCREEN);
    printf(HIDE_CURSOR);
    init = 1;

    return 0;
}

// destroy_window: free the memory contained in the window
void destroy_window(void) {
    if (!init) return;

    for (int i = 0; i < window.height * window.width; i++)
        free(window.pixels[i]);
    free(window.pixels);

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
    for (int i = 0; i < window.height * window.width; i++)
        strcpy(window.pixels[i], pattern[0]);
    printf(CURSOR_HOME);
}

#define CAMERA_Z -10

// plot : put vertex in pixels buffer
void plot(vertex_t v) {
    // projected coordinated on a the image plane
    double projx = (v.x * focal_length) / (v.z - CAMERA_Z);
    double projy = (v.y * focal_length) / (v.z - CAMERA_Z);

    // convert to pixel coordinates
    double pxx = (window.width / 2) + projx;
    double pxy = (window.height / 2) - projy;

    // array index
    int index = (window.width * (int) pxy) + (int) pxx;
    if (index > -1 && index < window.height * window.width) // quick bounds check
        strcpy(window.pixels[index], pattern[9]);
}

#define NUMPIXELSINDEX   0

// bresenham : Draw a line between vertices
unsigned int *bresenham(vertex_t v0, vertex_t v1) {
    unsigned int maxentries = (window.height > window.width) ? window.height : window.width;
    unsigned int *linepixels = (unsigned int *) malloc(sizeof(unsigned int) * maxentries + 1);
    unsigned int lppos = 1;
    if (linepixels == NULL)
        return NULL;

    linepixels[NUMPIXELSINDEX] = 0;  // First index used to track num elements

    // convert both vertexes into pixel coordinates
    int pxx0 = (window.width / 2)  + (v0.x * focal_length) / (v0.z - CAMERA_Z);
    int pxy0 = (window.height / 2) - (v0.y * focal_length) / (v0.z - CAMERA_Z);
    int pxx1 = (window.width / 2)  + (v1.x * focal_length) / (v1.z - CAMERA_Z);
    int pxy1 = (window.height / 2) - (v1.y * focal_length) / (v1.z - CAMERA_Z);

    int dx = abs(pxx1 - pxx0);
    int dy = abs(pxy1 - pxy0);

    // Determining the direction of the line
    int xstep = (pxx0 < pxx1) ? 1 : -1;
    int ystep = (pxy0 < pxy1) ? 1 : -1;

    int error = dx - dy;
    int x = pxx0;
    int y = pxy0;
    for (;;) {
        // Bounds check before calculating index
        if (x >= 0 && x < window.width && y >= 0 && y < window.height) {
            int index = (window.width * y) + x;
            linepixels[lppos++] = index;
            linepixels[NUMPIXELSINDEX] += 1;
        }

        if (x == pxx1 && y == pxy1) break;
        int error2 = 2 * error;

        if (error2 > -dy) { // step in the x direction
            error -= dy;
            x += xstep;
        }

        if (error2 < dx) {  // step in the y direction
            error += dx;
            y += ystep;
        }
    }

    linepixels = (unsigned int *) realloc(linepixels, sizeof(unsigned int) * linepixels[0] + 1);
    return linepixels;
}

// intncpy : copy n integer values from src to dest
void intncpy(unsigned int *dest, unsigned int *src, unsigned int n) {
    if (dest == NULL || src == NULL) return;
    for (; n > 0; --n) *dest++ = *src++;
}

// compint : comparison function that returns
//           negative : if a < b
//           zero     : if a == b
//           positive : if a > b
int compint(const int *a, const int *b) {
    return a - b;
}

// swap : swap values of the indexes a and b
void swap(unsigned int *a, unsigned int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// quicksort : sorts the elements of an unsigned integer array
void quicksort(unsigned int *arr, unsigned int low, unsigned int high) {
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

// getedges : identifies the edges on the surface and returns a sorted array with all surfaces pixels
unsigned int *getedges(vertex_t v0, vertex_t v1, vertex_t v2) {
    unsigned int *line1 = bresenham(v0, v1);
    unsigned int *line2 = bresenham(v1, v2);
    unsigned int *line3 = bresenham(v0, v2);
    unsigned int totalv = line1[NUMPIXELSINDEX] + line2[NUMPIXELSINDEX] + line3[NUMPIXELSINDEX];

    unsigned int *surfaceedges = (unsigned int *) malloc(sizeof(unsigned int) * totalv);
    if (surfaceedges == NULL) {
        free(line1);
        line1 = NULL;
        free(line2);
        line2 = NULL;
        free(line3);
        line3 = NULL;
        return NULL;
    }

    intncpy(surfaceedges, &line1[1], line1[NUMPIXELSINDEX]);
    intncpy(surfaceedges + line1[NUMPIXELSINDEX], &line2[1], line2[NUMPIXELSINDEX]);
    intncpy(surfaceedges + line1[NUMPIXELSINDEX] + line2[NUMPIXELSINDEX], &line3[1], line3[NUMPIXELSINDEX]);

    quicksort(surfaceedges, 0, totalv - 1);

    free(line1);
    line1 = NULL;
    free(line2);
    line2 = NULL;
    free(line3);
    line3 = NULL;

    return surfaceedges;
}

// draw_surface : draws the surface based on the face definition
void draw_surface(vertex_t v0, vertex_t v1, vertex_t v2) {
    unsigned int *edgetable = getedges(v0, v1, v2);

    // Scanline Algorithm

    free(edgetable);
    edgetable = NULL;
}