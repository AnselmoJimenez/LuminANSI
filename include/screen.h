// init_window : initialize a window of size height and width
int init_window(const int height, const int width);

// destroy_window: free the memory contained in the window
void destroy_window(void);

// keypress : returns keypress or -1 if no key was pressed
int keypress(void);

// draw_window : draw the buffer of characters
void draw_window(void);

// clear_window : reset the window pixels
void clear_window(void);

// draw_surface : draws the surface based on the face definition
void draw_surface(vertex_t v0, vertex_t v1, vertex_t v2, vertex_t vn);