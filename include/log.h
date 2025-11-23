#ifndef LOG_H
#define LOG_H

enum log_level { 
    INFO, 
    ERROR 
};

// init_log : initialize the file pointer for the logger
int init_log(const char *);

// close_log : closes the log file pointer
int close_log(void);

// log_message : logs message
void log_message(enum log_level, const char *, ...);

#endif