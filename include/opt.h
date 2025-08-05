/* opt.h : My version of getopt */

extern char *optarg; // pointer to the option's argument (when option takes an argument)
extern int optind;   // index of next element in argv to process
extern int opterr;   // if non-zero, getopt prints error messages (default is 1)
extern char opterrc; // contains the flag character when an error occurs

// getoption : returns current option or -1 if none left
int getopt(int, const char **, const char *);