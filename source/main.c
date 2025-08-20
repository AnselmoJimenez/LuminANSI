#include <stdio.h>

#include "../include/object.h"
#include "../include/screen.h"
#include "../include/opt.h"

int main(int argc, char const *argv[]) {
    const int height = 64;
    const int width = 256;
    double rotation_angle = 0;
    FILE *fp;

    int option = getopt(argc, argv, "hf:");
    switch (option) {
        case 'f':
            if (optarg == NULL) {
                printf(USAGE);
                return 1;
            }
            else if ((fp = fopen(optarg, "r")) == NULL)
                return 1;
            break;
        case 'h':
        default:
            printf(USAGE);
            return 1;
    }

    if (init_window(height, width) != 0)
        return 1;
    
    object_t obj = load(fp);
    vertex_t transforms[MAXVERTICES];

    for (int count = 0; count < 500; count++) {
        clear_window();

        rotation_angle += 0.005;

        for (int i = 0; i < 45; i++) {
            transforms[i] = obj.vertices[i];
            rotate_y(rotation_angle, &transforms[i]);
            rotate_x(rotation_angle, &transforms[i]);
            rotate_z(rotation_angle, &transforms[i]);
            plot(transforms[i]);
        }

        draw_window();
    }

    destroy_window();
    fclose(fp);
    return 0;
}