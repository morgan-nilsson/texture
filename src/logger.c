#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "../include/logger.h"

void logger_add(struct Logger* logger, const char* format, ...) {
    // logging must never take the editor down, drop the message if the log didn't open
    if (logger->file == NULL) return;
    va_list args;
    va_start(args, format);
    vfprintf(logger->file, format, args);
    fprintf(logger->file, "\n");
    fflush(logger->file);
    va_end(args);
}

void logger_warn(struct Logger* logger, const char* format, ...) {
    // logging must never take the editor down, drop the message if the log didn't open
    if (logger->file == NULL) return;
    va_list args;
    va_start(args, format);
    fprintf(logger->file, "|WARNING| ");
    vfprintf(logger->file, format, args);
    fprintf(logger->file, "\n");
    fflush(logger->file);
    va_end(args);
}

void logger_error(struct Logger* logger, const char* format, ...) {
    // logging must never take the editor down, drop the message if the log didn't open
    if (logger->file == NULL) return;
    va_list args;
    va_start(args, format);
    fprintf(logger->file, "|ERROR| ");
    vfprintf(logger->file, format, args);
    fprintf(logger->file, "\n");
    fflush(logger->file);
    va_end(args);
}

void closeLogger(struct Logger* logger) {
    if (logger->file != NULL) {
        fflush(logger->file);
        fclose(logger->file);
    }
    free(logger);
}

struct Logger* initLogger(const char* filename) {
    struct Logger* logger = malloc(sizeof(struct Logger));
    if (logger == NULL) return NULL;
    logger->file = fopen(filename, "a");
    logger->add = &logger_add;
    logger->warn = &logger_warn;
    logger->error = &logger_error;
    logger->close = &closeLogger;
    return logger;
}
