/* opt.h : My version of getopt */

#define USAGE "usage:\n" \
"\th : displays this message\n" \
"\tx : Enables x rotation\n" \
"\ty : Enables y rotation\n" \
"\tz : Enables z rotation\n" \
"\tf [file path] : path to object file\n" \
"\td [HEIGHTxWIDTH] : overwrite screen resolution\n"

extern char *optarg; // pointer to the option's argument (when option takes an argument)
extern int optind;   // index of next element in argv to process
extern int opterr;   // if non-zero, getopt prints error messages (default is 1)
extern char opterrc; // contains the flag character when an error occurs

// getoption : returns current option or -1 if none left
int getopt(int argc, const char **argv, const char *argoptions);