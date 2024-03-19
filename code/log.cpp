#include "log.h"

// ----------------------------------------------------------------------------
// Module variables
// ----------------------------------------------------------------------------

Print * _log_output;
unsigned char _log_minlevel = LOG_INFO;

// ----------------------------------------------------------------------------
// Entry points
// ----------------------------------------------------------------------------

void logSetup(Print * output) {
    _log_output = output;
}

void logLevel(unsigned char minlevel) {
    _log_minlevel = minlevel;
}

void logWrite(unsigned char level, const char * type, const char * message, ...) {
    
    if (level > _log_minlevel) return;
    
    char tmp[256];
    va_list  args;
    va_start (args, message);
    vsprintf(tmp, message, args);
    va_end (args);
    
    char output[256];
    sprintf(output, "[%09.3f] - %-4s - %s", millis() / 1000.0, type, tmp);
    _log_output->println(output);
    delay(1);

}

void logWriteRaw(unsigned char level, const char * message, ...) {
    
    if (level > _log_minlevel) return;
    
    char tmp[256];
    va_list  args;
    va_start (args, message);
    vsprintf(tmp, message, args);
    va_end (args);
    
    _log_output->print(tmp);
    delay(1);

}

