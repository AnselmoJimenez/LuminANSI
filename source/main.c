#include <stdio.h>

#include "../include/mesh.h"
#include "../include/screen.h"
#include "../include/opt.h"
#include "../include/graphics.h"

#define DEBUG
#ifdef DEBUG
void print_surfaces(mesh_t mesh) {
    for (int i = 0; i < mesh.sfcount; i++) {
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
    char *filename;
    int height = 128;
    int width = 512;

    if (!init_log("system.log")) return 1;
    if (!argparse(argc, argv, &filename, &height, &width)) return 1;

    const mesh_t mesh = load(filename);
    screen_t *screen = init_screen(height, width);
    if (screen == NULL) 
        return 1;

    render(screen, mesh);

    destroy_screen(screen);
    close_log();

    return 0;
}