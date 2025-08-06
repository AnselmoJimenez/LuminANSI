#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// valid : returns 0 or 1 if character is in string
int valid(char c, char *str) {
    for (*str; *str != '\0'; str++)
        if (*str == c) return 0;
    return 1;
}

// continuous : returns 0 or 1 if character has a : after in str
int continuous(char c, char *str) {
    for (*str; *str != '\0' && *str != c; str++)
        ;
    if (*str == '\0') return 1;
    if (*++str == ':') return 0;
    else return 1;
}

#define MAXSTRING   25

char *optarg;
int optind;
int opterr = 1;
char opterrc;

// getopt : returns current option or -1 if none left
int getopt(int argc, const char **argv, const char *argoptions) {
    optarg = 0;
    optind = 1;

    char option = 0;
    while (argv[optind]) {
        option = *(argv + optind)[0] == '-' ? *(argv + optind)[1] : 0;

        // Check if valid option
        if (!valid(option, argoptions)) {
            opterrc = option;
            if (!opterr) printf("getopt: error: invalid option -%c\n", opterrc);
            return -1;
        }

        // Check for option argument
        if (continuous(option, argoptions)) {
            optarg = (char) malloc(MAXSTRING * sizeof(char));
            strcpy(optarg, argv[++optind]);
        }

        ++optind;
        return option;
    }
}