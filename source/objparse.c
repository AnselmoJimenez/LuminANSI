#include <stdio.h>

#include "../include/objparse.h"

// Vertex information
char *object_name;
vertex_t vertices[MAXVERTICES];
float vnormals[MAXVERTICES] = { 0.0 };
int fdefinitions[MAXVERTICES] = { 0 };

int gettoken(char *string, int length);

// load : load the object file name and store
void load(char *filename) {

}

// gettoken : get next token
int gettoken(char *string, int length) {

}