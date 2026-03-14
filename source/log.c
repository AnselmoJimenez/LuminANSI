#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#include "../include/log.h"

static char *log_levels[] = {
    "INFO",
    "ERROR"
};

static FILE *log;

// init_log : initialize the file pointer for the logger
int init_log(const char *filename) {
    log = fopen(filename, "w");
    if (log == NULL) {
        printf("Unable to initialize logging system", filename);
        return 0;
    }
    return 1;
}

// close_log : closes the log file pointer
int close_log(void) {
    fclose(log);
    return 0;
}

// log_message : logs message
void log_message(enum log_level level, const char *message, ...) {
    time_t now = time(NULL);
    char *current_time = ctime(&now);

    // removing '\n' from end of line
    current_time[strlen(current_time) - 1] = '\0';

    fprintf(log, "[%s] [%s] : ", current_time, log_levels[level]);

    // Handle variable arguments
    va_list arguments;
    va_start(arguments, message);
    vfprintf(log, message, arguments);
    va_end(arguments);

    fprintf(log, "\n");
}