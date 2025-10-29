#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/opt.h"

// valid : returns 0 or 1 if character is in string
int valid(char c, const char *str) {
    for ( ; *str != '\0'; str++)
        if (*str == c) return 1;
    return 0;
}

// continuous : returns 0 or 1 if character has a : after in str
int continuous(char c, const char *str) {
    for ( ; *str != '\0' && *str != c; str++)
        ;
    if (*str == '\0') return 0;
    if (*++str == ':') return 1;
    else return 0;
}

#define MAXSTRING   25

char *optarg = NULL;
int optind = 1;
int opterr = 1;
char opterrc = 0;

// getopt : returns current option or -1 if none left
int getopt(int argc, const char **argv, const char *argoptions) {
    optarg = NULL;

    char option = 0;
    while (optind < argc && argv[optind]) {
        option = (*(argv + optind))[0] == '-' ? (*(argv + optind))[1] : 0;

        // Check if valid option
        if (!valid(option, argoptions)) {
            opterrc = option;
            if (opterr) printf("getopt: error: invalid option -%c\n", opterrc);
            return -1;
        }

        // Check for option argument
        if (continuous(option, argoptions)) optarg = (char *) argv[++optind];

        ++optind;
        return option;
    }

    return -1;
}