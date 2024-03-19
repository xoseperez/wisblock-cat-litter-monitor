#ifndef _LOG_H
#define _LOG_H

#include <inttypes.h>
#include <stdarg.h>
#include <Arduino.h>

enum {
    LOG_NONE = 0,
    LOG_ERROR,
    LOG_INFO,
    LOG_DEBUG,
    LOG_VERBOSE
};

void logSetup(Print * output);
void logLevel(unsigned char minlevel = LOG_INFO);
void logWrite(unsigned char level, const char * type, const char * message, ...);
void logWriteRaw(unsigned char level, const char * message, ...);

#endif // _LOG_H