#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "../include/mesh.h"
#include "../include/screen.h"

// new_vertex : create and return a new vertex object
vertex_t new_vertex(float x, float y, float z) { 
    return (vertex_t) { x, y, z };
}

#define TRIANGLEVERTICES 3

// roll : rotate the vertex coordinate by angle in the x direction about c
void roll(float angle, surface_t *surface) {
    vertex_t *vertices[TRIANGLEVERTICES] = { &surface->v0, &surface->v1, &surface->v2 };
    for (int i = 0; i < TRIANGLEVERTICES; i++) {
        float y = vertices[i]->y;
        float z = vertices[i]->z;
        vertices[i]->y = y * cos(angle) - z * sin(angle);
        vertices[i]->z = y * sin(angle) + z * cos(angle);
    }
}

// pitch : rotate the vertex coordinate by angle in the y direction about c
void pitch(float angle, surface_t *surface) {
    vertex_t *vertices[TRIANGLEVERTICES] = { &surface->v0, &surface->v1, &surface->v2 };
    for (int i = 0; i < TRIANGLEVERTICES; i++) {
        float x = vertices[i]->x;
        float z = vertices[i]->z;
        vertices[i]->x =  x * cos(angle) + z * sin(angle);
        vertices[i]->z = -x * sin(angle) + z * cos(angle);
    }
}

// yaw : rotate the vertex coordinate by angle in the z direction about c
void yaw(float angle, surface_t *surface) {
    vertex_t *vertices[TRIANGLEVERTICES] = { &surface->v0, &surface->v1, &surface->v2 };
    for (int i = 0; i < TRIANGLEVERTICES; i++) {
        float x = vertices[i]->x;
        float y = vertices[i]->y;
        vertices[i]->x = x * cos(angle) - y * sin(angle);
        vertices[i]->y = x * sin(angle) + y * cos(angle);
    }
}

#define MAXSTRINGLEN 512    

// parse_stl : parse STL (.stl) file formats
static void parse_stl(FILE *fp, mesh_t *mesh) {
    // TODO: Implement STL file parsing
}

// advance : advance the line pointer to the next face definition
static void advance(char **lp) {
    while (**lp != ' ' && **lp != '\n') (*lp)++;
    if (**lp == ' ') ++(*lp);
}

// parse_wavefront : parse wavefront (.obj) file formats
static void parse_wavefront(FILE *fp, mesh_t *mesh) {
    char line[MAXSTRINGLEN] = { '\0' };

    while (fgets(line, MAXSTRINGLEN, fp) != NULL) {
        switch (line[0]) {
            case 'v':
                if (mesh->vcount >= MAXVERTICES) {
                    printf("LuminANSI: Vertex Count Limit reached\n");
                    break;
                }
                float x, y, z;
                if (sscanf(line, "v %f %f %f\n", &x, &y, &z) != 3) break;
                mesh->vertices[mesh->vcount++] = new_vertex(x, y, z);
            break;

            case 'f':
                if (mesh->fcount >= EULERSPOLYHEDRA) {
                    printf("LuminANSI: Surface Count Limit Reached\n");
                    break;
                }
                int root_v = 0;
                int last_v = 0;
                sscanf(line, "f %d/%*s %d", &root_v, &last_v);
                
                // skip to next vertex and begin parsing
                char *lp = &line[2];
                advance(&lp);
                advance(&lp);

                while (*lp != '\n' && *lp != '\0' && *lp != EOF) {
                    if (mesh->fcount >= EULERSPOLYHEDRA) {
                        printf("LuminANSI: Surface Count Limit Reached\n");
                        break;
                    }
                    mesh->surfaces[mesh->fcount].v0 = mesh->vertices[root_v - 1];
                    mesh->surfaces[mesh->fcount].v1 = mesh->vertices[last_v - 1];

                    sscanf(lp, "%d/%*s", &last_v);
                    mesh->surfaces[mesh->fcount].v2 = mesh->vertices[last_v - 1];
                    advance(&lp);
                    
                    if (mesh->fcount < EULERSPOLYHEDRA) {
                        ++mesh->fcount;
                    }
                }
            break;
            
            default: break;
        }
    }
}

// load : loads the object file name
mesh_t load(const char *filename, FILE *fp) {
    mesh_t mesh;
    mesh.fcount = 0;
    mesh.vcount = 0;
    if      (strstr(filename, ".obj\0")) parse_wavefront(fp, &mesh);
    else if (strstr(filename, ".stl\0")) parse_stl(fp, &mesh);

    return mesh;
}