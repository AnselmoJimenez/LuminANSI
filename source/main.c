#include <stdio.h>

#include "../include/object.h"
#include "../include/screen.h"
#include "../include/opt.h"

#define DEBUG
#ifdef DEBUG
void print_faces(object_t obj) {
    for (int i = 0; i < fcount; i++) {
        printf("face %d -\n\t(x: %lf, y: %lf, z: %lf)\n\t(x: %lf, y: %lf, z: %lf)\n\t(x: %lf, y: %lf, z: %lf)\n", 
            i, 
            obj.vertices[obj.faces[i].vertex_index[0] - 1].x, obj.vertices[obj.faces[i].vertex_index[0] - 1].y, obj.vertices[obj.faces[i].vertex_index[0] - 1].z,
            obj.vertices[obj.faces[i].vertex_index[1] - 1].x, obj.vertices[obj.faces[i].vertex_index[1] - 1].y, obj.vertices[obj.faces[i].vertex_index[1] - 1].z,
            obj.vertices[obj.faces[i].vertex_index[2] - 1].x, obj.vertices[obj.faces[i].vertex_index[2] - 1].y, obj.vertices[obj.faces[i].vertex_index[2] - 1].z
        );
    }
}
#endif

int main(int argc, char const *argv[]) {
    int height = 128;
    int width = 512;
    int x_rot = 0;
    int y_rot = 0;
    int z_rot = 0;
    double rotation_angle = 0;
    FILE *fp;

    int option = 0;
    while ((option = getopt(argc, argv, "hf:d:xyz")) != -1) {
        switch (option) {
            case 'f':
                if (optarg == NULL) {
                    printf(USAGE);
                    return 1;
                }
                else if ((fp = fopen(optarg, "r")) == NULL) {
                    printf("LuminANSI: Unable to open file '%s'\n", optarg);
                    return 1;
                }
                break;
            case 'd':
                int temp_height = 0;
                int temp_width = 0;
                if (optarg == NULL || sscanf(optarg, "%dx%d", &temp_height, &temp_width) != 2) {
                    printf(USAGE);
                    return 1;
                }
                if (temp_height < 0 || temp_width < 0) {
                    printf("LuminANSI: Negative dimensions not allowed\n");
                    return 1;
                }
                else if ((0 < temp_height && temp_height < 128) || (0 < temp_width && temp_width < 512)) {
                    printf("LuminANSI: Screen defaulting to 128x512\n");
                    break;
                }
                width = temp_width;
                height = temp_height;
                break;
            case 'x': x_rot = 1; break;
            case 'y': y_rot = 1; break;
            case 'z': z_rot = 1; break;
            case 'h':
            default:
                printf(USAGE);
                return 1;
        }
    }
    
    object_t obj = load(fp);
    fclose(fp);

    vertex_t transforms[MAXVERTICES];
    if (init_window(height, width) != 0) 
        return 1;

    for (;;) {
        switch (keypress()) {
            case 'q':
                destroy_window();
                return 0;
            default: break;
        }

        clear_window();

        rotation_angle += 0.005;

        for (int i = 0; i < vcount; i++) {
            transforms[i] = obj.vertices[i];
            if (x_rot) rotate_x(rotation_angle, &transforms[i]);
            if (y_rot) rotate_y(rotation_angle, &transforms[i]);
            if (z_rot) rotate_z(rotation_angle, &transforms[i]);
        }

        for (int i = 0; i < fcount; i++) {
            draw_surface(transforms[obj.faces[i].vertex_index[0] - 1], 
                        transforms[obj.faces[i].vertex_index[1] - 1], 
                        transforms[obj.faces[i].vertex_index[2] - 1]
                    );
        }
    
        draw_window();
    }

    // will never be reached but just for sanity's sake
    destroy_window();

    return 0;
}