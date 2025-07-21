// init_window : initialize a window of size height and width
int init_window(const int height, const int width);

// destroy_window: free the memory contained in the window
void destroy_window(void);

// draw_window : draw the buffer of characters
void draw_window(void);

// clear_window : reset the window pixels
void clear_window(void);

// plot : put vertex in screen buffer
void plot(vertex_t);

// connect : draw a line between vertices (Bresenham's Line Algorithm)
void connect(vertex_t v0, vertex_t v1);