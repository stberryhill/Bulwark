/*
 * License: Zlib
 * Copyright (c) Sean Tyler Berryhill (sean.tyler.berryhill@gmail.com)
 */

#include "Internal.h"

#include <stdarg.h>
#include <stdio.h>

#define LOGGING
#define LOG_FILENAME "log.txt"

/* Private variables */
static FILE *logFile;

/* Function definitions */
void Log_Open() {
  #ifdef LOGGING
    logFile = fopen("Log.txt", "w");

    if (!logFile) {
      printf("Error - could not open log file.");
    }
  #endif
}

void Log_Info(const char *message, ...) {
  #ifdef LOGGING
    va_list variableArgumentList;
    va_start(variableArgumentList, message);
    
    fprintf(logFile, "[Info] ");
    vfprintf(logFile, message, variableArgumentList);
    fprintf(logFile, "\n");
    fflush(logFile);

    va_end(variableArgumentList);
  #endif
}

void Log_Error(const char *message, ...) {
  #ifdef LOGGING
    va_list variableArgumentList;
    va_start(variableArgumentList, message);

    fprintf(logFile, "[Error] ");
    vfprintf(logFile, message, variableArgumentList);
    fprintf(logFile, "\n");
    fflush(logFile);

    va_end(variableArgumentList);
  #endif
}

void Log_Close() {
  #ifdef LOGGING
    fclose(logFile);
  #endif
}
