#include <stdio.h>

#include "../include/mesh.h"
#include "../include/screen.h"
#include "../include/opt.h"
#include "../include/graphics.h"
#include "../include/log.h"

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

    if (init_log("system.log") != 0) {
        printf("Unable to initialize log");
        return 1;
    }
    log_message(INFO, "Initializing LuminANSI");

    if (argparse(argc, argv, &filename, &height, &width) == -1) {
        log_message(ERROR, "LuminANSI: Unable to parse command line arguments");
        return 1;
    }

    log_message(INFO, "Reading %s mesh data...", filename);
    const mesh_t mesh = load(filename);
    if (mesh.sfcount == 0) {
        log_message(ERROR, "LuminANSI: Unable to read %s mesh data", filename);
        return 1;
    }
    log_message(INFO, "%s mesh data parsing complete.", filename);
    log_message(INFO, "\t%d vertices", mesh.vcount);
    log_message(INFO, "\t%d surfaces", mesh.sfcount);

    log_message(INFO, "Allocating Memory for screen...");
    screen_t *screen = init_screen(height, width);
    if (screen == NULL) {
        log_message(ERROR, "LuminANSI: Unable to allocate screen memory");
        return 1;
    }
    log_message(INFO, "Memory allocation successful.");

    log_message(INFO, "Rendering...");
    render(screen, mesh);

    log_message(INFO, "Exit");
    destroy_screen(screen);
    close_log();

    return 0;
}