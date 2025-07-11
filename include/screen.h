// init_window : initialize a window of size height and width
int init_window(const int height, const int width);

// destroy_window: free the memory contained in the window
void destroy_window(void);

// draw_window : draw the buffer of characters
int draw_window(void);

// plot : put vertex in screen buffer
void plot(vertex_t);
