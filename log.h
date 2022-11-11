#ifndef __LOG_H
#define __LOG_H 1

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <assert.h>

#include <pthread.h>
#include <sched.h>
#include <unistd.h>
#include <stdint.h>
#include <signal.h>

#include <sys/mman.h>
#include <errno.h>
#include <stdarg.h>

void log_error(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    fprintf(stdout, "ERROR: ");
    vfprintf(stdout, fmt, ap);
    fprintf(stdout, "\n");
    va_end(ap);
}

void log_warning(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    fprintf(stdout, "WARNING: ");
    vfprintf(stdout, fmt, ap);
    fprintf(stdout, "\n");
    va_end(ap);
}

#endif /* __LOG_H */
