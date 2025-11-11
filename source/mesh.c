#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdint.h>

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

#define HEADERSIZE      80
#define TRIANGLESPEC    36
#define NORMALSIZE      12
#define ATTRSIZE        2

// TODO: Implement check for big endian systems
// parse_stl : parse STL (.stl) file formats
static void parse_stl(FILE *fp, mesh_t *mesh) {
    // skip 80 byte header
    if (fseek(fp, HEADERSIZE, SEEK_SET) == -1) {
        printf("Error reading file");
        return; 
    }

    // read the number of triangles in the mesh
    fread(&mesh->sfcount, sizeof(int), 1, fp);
    if (mesh->sfcount > EULERSPOLYHEDRA) {
        printf("Too many surfaces. Aborting\n");
        return;
    }
    
    // loop and parse all of the triangle data
    for (int i = 0; i < mesh->sfcount; i++) {
        if (fseek(fp, NORMALSIZE, SEEK_CUR) == -1) {
            printf("Error reading file");
            return; 
        }

        fread(&mesh->surfaces[i], sizeof(surface_t), 1, fp);

        if (fseek(fp, ATTRSIZE, SEEK_CUR) == -1) {
            printf("Error reading file");
            return; 
        }
    }
}

#define MAXSTRINGLEN 512   

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
                if (mesh->sfcount >= EULERSPOLYHEDRA) {
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
                    if (mesh->sfcount >= EULERSPOLYHEDRA) {
                        printf("LuminANSI: Surface Count Limit Reached\n");
                        break;
                    }
                    mesh->surfaces[mesh->sfcount].v0 = mesh->vertices[root_v - 1];
                    mesh->surfaces[mesh->sfcount].v1 = mesh->vertices[last_v - 1];

                    sscanf(lp, "%d/%*s", &last_v);
                    mesh->surfaces[mesh->sfcount].v2 = mesh->vertices[last_v - 1];
                    advance(&lp);
                    
                    if (mesh->sfcount < EULERSPOLYHEDRA) {
                        ++mesh->sfcount;
                    }
                }
            break;
            
            default: break;
        }
    }
}

// load : loads the object file name
mesh_t load(const char *filename) {
    mesh_t mesh;
    mesh.sfcount = 0;
    mesh.vcount = 0;

    FILE *fp;
    if ((fp = fopen(filename, "r")) == NULL) {
        printf("Unable to open %s\n", filename);
        return mesh;
    }

    if      (strstr(filename, ".obj\0")) parse_wavefront(fp, &mesh);
    else if (strstr(filename, ".stl\0")) parse_stl(fp, &mesh);

    fclose(fp);
    
    return mesh;
}