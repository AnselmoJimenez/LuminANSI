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
    const int height = 450;
    const int width = 1024;
    double rotation_angle = 0;
    FILE *fp;

    int option = getopt(argc, argv, "hf:");
    switch (option) {
        case 'f':
            if (optarg == NULL) {
                printf(USAGE);
                return 1;
            }
            else if ((fp = fopen(optarg, "r")) == NULL) {
                printf("asciipoly: Unable to open file '%s'\n", optarg);
                return 1;
            }
            break;
        case 'h':
        default:
            printf(USAGE);
            return 1;
    }

    if (init_window(height, width) != 0) return 1;
    
    object_t obj = load(fp);
    vertex_t transforms[MAXVERTICES];

    for (;;) {
        switch (keypress()) {
            case 'q':
                destroy_window();
                fclose(fp);
                return 0;
            default: break;
        }

        clear_window();

        rotation_angle += 0.01;

        for (int i = 0; i < vcount; i++) {
            transforms[i] = obj.vertices[i];
            rotate_z(rotation_angle, &transforms[i]);
            rotate_y(rotation_angle, &transforms[i]);
            // rotate_x(rotation_angle, &transforms[i]);
        }

        for (int i = 0; i < fcount; i++) {
            draw_surface(transforms[obj.faces[i].vertex_index[0] - 1], 
                    transforms[obj.faces[i].vertex_index[1] - 1], 
                    transforms[obj.faces[i].vertex_index[2] - 1],
                    obj.normals[obj.faces[i].normal_index]);
        }
    
        draw_window();
    }

    // will never be reached but just for sanity's sake
    destroy_window();
    fclose(fp);

    return 0;
}