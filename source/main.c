#include <stdio.h>

#include "../include/mesh.h"
#include "../include/screen.h"
#include "../include/opt.h"
#include "../include/graphics.h"

#define DEBUG
#ifdef DEBUG
void print_surfaces(mesh_t mesh) {
    for (int i = 0; i < mesh.fcount; i++) {
        printf("face %d -\n\t(x: %f, y: %f, z: %f)\n\t(x: %f, y: %f, z: %f)\n\t(x: %f, y: %f, z: %f)\n", 
            i, 
            mesh.surfaces[i].v0.x, mesh.surfaces[i].v0.y, mesh.surfaces[i].v0.z,
            mesh.surfaces[i].v1.x, mesh.surfaces[i].v1.y, mesh.surfaces[i].v1.z,
            mesh.surfaces[i].v2.x, mesh.surfaces[i].v2.y, mesh.surfaces[i].v2.z
        );
    }
}
#endif

int main(int argc, char const *argv[]) {
    FILE *fp;
    const char *filename;
    int height = 128;
    int width = 512;
    float rotation_angle = 0;

    int option = 0;
    while ((option = getopt(argc, argv, "hf:d:")) != -1) {
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
                filename = optarg;
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
            case 'h':
            default:
                printf(USAGE);
                return 1;
        }
    }
    

    const mesh_t mesh = load(filename, fp);
    fclose(fp);

    screen_t *screen = init_screen(height, width);
    if (screen == NULL) {
        printf("LuminANSI: Unable to allocate screen\n");
        return 1;
    }

    for (;;) {
        switch (keypress()) {
            case 'q':
                destroy_screen(screen);
                return 0;
            default: break;
        }

        clear_screen(screen);      

        rotation_angle += 0.01;
        for (int i = 0; i < mesh.fcount; i++)
            draw_surface(screen, mesh.surfaces[i], rotation_angle);
    
        draw_screen(screen);
    }

    // will never be reached but just for sanity's sake
    destroy_screen(screen);

    return 0;
}