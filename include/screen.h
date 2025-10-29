#ifndef SCREEN_H
#define SCREEN_H

typedef struct screen screen_t;

// init_screen : initialize a window of size height and width
screen_t *init_screen(const int height, const int width);

// destroy_screen: free the memory contained in the window
void destroy_screen(screen_t *screen);

// draw_screen : draw the buffer of characters
void draw_screen(screen_t *screen);

// clear_screen : reset the window pixels
void clear_screen(screen_t *screen);

// keypress : returns keypress or -1 if no key was pressed
int keypress(void);

#endif