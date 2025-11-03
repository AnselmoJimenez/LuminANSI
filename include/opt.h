/* opt.h : My version of getopt */

#ifndef OPT_H
#define OPT_H

#define USAGE "usage:\n" \
"\th : displays this message\n" \
"\tf [file path] : path to object file\n" \
"\td [HEIGHTxWIDTH] : overwrite screen resolution\n"

#define OPTIONS "hf:d:"

// argparse : parse the command line arguments
int  argparse(int argc, const char **argv, char **filename, int *height, int *width);

#endif