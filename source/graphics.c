#include <string.h>
#include <math.h>

#include "../include/graphics.h"
#include "../include/util.h"
#include "../include/mesh.h"

static const char *pattern[PATTERNSIZE] = {
    "\x1b[38;5;232m█\x1b[0m",  // 1  - Darkest
    "\x1b[38;5;233m█\x1b[0m",  // 2
    "\x1b[38;5;235m█\x1b[0m",  // 3
    "\x1b[38;5;237m█\x1b[0m",  // 4
    "\x1b[38;5;239m█\x1b[0m",  // 5
    "\x1b[38;5;241m█\x1b[0m",  // 6
    "\x1b[38;5;243m█\x1b[0m",  // 7
    "\x1b[38;5;245m█\x1b[0m",  // 8  - Middle
    "\x1b[38;5;247m█\x1b[0m",  // 9
    "\x1b[38;5;249m█\x1b[0m",  // 10
    "\x1b[38;5;251m█\x1b[0m",  // 11
    "\x1b[38;5;252m█\x1b[0m",  // 12
    "\x1b[38;5;253m█\x1b[0m",  // 13
    "\x1b[38;5;254m█\x1b[0m",  // 14
    "\x1b[38;5;255m█\x1b[0m"   // 15 - Brightest white
};

#define CAMERA_Z -8

#define MAX(a, b) ((a) > (b)) ? (a) : (b)
#define MIN(a, b) ((a) < (b)) ? (a) : (b)

// new_pixel : returns a new pixel
pixel_t new_pixel(int x, int y, float z, char* c) {
    pixel_t newpx = { .x = x, .y = y, .z = z };
    strncpy(newpx.c, c, STRINGSIZE);
    return newpx;
}

// wtopx : converts 3D world coordinates to pixel coordinates
static pixel_t wtopx(screen_t *screen, vertex_t v, char c[PATTERNSIZE]) {
    return new_pixel(
        (int) ((screen->width / 2)  + (v.x * screen->focal_length) / (v.z - CAMERA_Z)),
        (int) ((screen->height / 2) - (v.y * screen->focal_length) / (v.z - CAMERA_Z)),
        (float) v.z,
        c
    );
}

// pxtow : Convert pixel coordinates to screen coordinates
static int pxtoscr(screen_t *screen, pixel_t px) {
    return (screen->width * px.y) + px.x;
}

// plotpx : Plot pixel in pixels buffer
static void plotpx(screen_t *screen, pixel_t px) {
    // array index
    int index = pxtoscr(screen, px);
    if (index > -1 && index < screen->height * screen->width) {
        if (px.z <= screen->pixels[index].z) {
            screen->pixels[index].x = px.x;
            screen->pixels[index].y = px.y; 
            screen->pixels[index].z = px.z;
            strcpy(screen->pixels[index].c, px.c);
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
static void fill_surface(screen_t *screen, intersection_t intersections, int y) {
    // calculate deltas
    int dx = intersections.right.x - intersections.left.x;
    float dz = intersections.right.z - intersections.left.z;
    
    // Interpolate through z
    for (int x = intersections.left.x; x < intersections.right.x; x++) {
        float t = (float) (x - intersections.left.x) / dx;
        float z = intersections.left.z + t * dz;

        pixel_t px = new_pixel(x, y, z, intersections.left.c);
        plotpx(screen, px);
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
        hits[count++] = new_pixel(x, y, z, px_start.c);
    }

    // sort by x values
    if (count == 2 && hits[0].x > hits[1].x)
        swap(&hits[0], &hits[1]);
    
    result.left = hits[0];
    result.right = (count == 2) ? hits[1] : hits[0];

    return result;
}

// get_brightness : calculates the brightness of a surface
static float get_brightness(surface_t surface) {
    // calculate lighting
    vertex_t light = new_vertex(0.5, 1, -0.5);
    vertex_t light_direction = normalize(light);
    vertex_t surface_normal  = normalize(cross(vsubtract(surface.v1, surface.v0), vsubtract(surface.v2, surface.v0)));

    float ambient_light = 0.1f; // minimum brightness
    float diffuse = MAX(0, dot(surface_normal, light_direction));
    return ambient_light + (1.0f - ambient_light) * diffuse;
}

// draw_surface : draws the surface based on the face definition
void draw_surface(screen_t *screen, const surface_t surface, float theta) {
    surface_t transform;
    memcpy(&transform, &surface, sizeof(surface_t));
    roll(theta, &transform);
    pitch(theta, &transform);

    float intensity = get_brightness(transform);
    int index = (int) MAX(0, MIN(PATTERNSIZE - 1, intensity * (PATTERNSIZE - 1)));
    char *c = (char *) pattern[intensity < 0 ? 0 : index];  // Handle backfaces

    // Initialize vertex screen coordinates
    pixel_t px0 = wtopx(screen, transform.v0, c);
    pixel_t px1 = wtopx(screen, transform.v1, c);
    pixel_t px2 = wtopx(screen, transform.v2, c);
    pixel_t endpoints[3] = { px0, px1, px2 };

    // Find MAX and MIN y value for the current surface
    int min_y = MIN(px0.y, MIN(px1.y, px2.y));
    int max_y = MAX(px0.y, MAX(px1.y, px2.y));

    // scan line, find intersections, and fill based on z coordinate
    for (int y = min_y; y <= max_y; y++) {
        intersection_t intersect = get_intersection(endpoints, y);

        // then fill scanline with pixel coordinates
        fill_surface(screen, intersect, y);
    }
}