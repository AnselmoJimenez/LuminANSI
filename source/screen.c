#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <float.h>

#include "../include/graphics.h"
#include "../include/mesh.h"
#include "../include/screen.h"
#include "../include/util.h"

static struct termios original_state;

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

#define CURSOR_HOME     "\x1B\x5B\x48"
#define CLEAR_SCREEN    "\x1B\x5B\x32\x4A"
#define HIDE_CURSOR     "\x1B\x5B\x3F\x32\x35\x6C"
#define SHOW_CURSOR     "\x1B\x5B\x3F\x32\x35\x68"

// init_window : initialize a window of size height and width
screen_t *init_screen(const int height, const int width) {
    enable_raw_mode();

    screen_t *screen = (screen_t *) malloc(sizeof(screen_t));
    if (screen == NULL) 
        return NULL;

    // initialize screen variables
    screen->initialization = 1;
    screen->height = height;
    screen->width = width;
    screen->focal_length = screen->width / 2;
    screen->pixels = (pixel_t *) malloc((screen->height * screen->width) * sizeof(pixel_t));
    if (screen->pixels == NULL) 
        return NULL;
    
    // initialize the screen
    for (int i = 0; i < screen->height * screen->width; i++) {
        screen->pixels[i].x = INFINITE;
        screen->pixels[i].y = INFINITE;
        screen->pixels[i].z = FLT_MAX;
        strncpy(screen->pixels[i].c, " ", STRINGSIZE);
    }

    printf(CLEAR_SCREEN);
    printf(HIDE_CURSOR);

    return screen;
}

// destroy_window: free the memory contained in the window
void destroy_screen(screen_t *screen) {
    if (!screen->initialization) return;

    // free the pixels
    free(screen->pixels);
    free(screen);

    disable_raw_mode();
    printf(SHOW_CURSOR);
}

// draw_screen : draw the buffer of characters
void draw_screen(screen_t *screen) {
    for (int i = 0; i < screen->height * screen->width; i++) {
        if (i % screen->width == 0 && i != 0) printf("\n");
        printf("%s", screen->pixels[i].c);
    }
    printf("\n");
}

// clear_screen : reset the window pixels
void clear_screen(screen_t *screen) {
    for (int i = 0; i < screen->height * screen->width; i++) {
        screen->pixels[i].x = INFINITE;
        screen->pixels[i].y = INFINITE;
        screen->pixels[i].z = FLT_MAX;
        strncpy(screen->pixels[i].c, " ", STRINGSIZE);
    }
    printf(CURSOR_HOME);
}

// keypress : returns keypress or -1 if no key was pressed
int keypress(void) {
    char c;
    if (read(STDIN_FILENO, &c, 1) == 1) 
        return c;
    return -1;
}