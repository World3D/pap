#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "base/io.h"

#if defined(__WINDOWS__)
#ifndef vsnprintf
#define vsnprintf _vsnprintf
#endif
#endif

void baseio_perror(const char* format, ...) {
  char* newformat = NULL;
  uint32_t newformat_length = 0;
  uint32_t extendlength = 0;
  char buffer[2048];
#if defined(__LINUX__) /* { */
  const char* formathead = "\e[0;31;1m";
  const char* formatend = "\e[0m\n";
#elif defined(__WINDOWS__) /* }{ */
  const char* formathead = "(error)->";
  const char* formatend = "<-(error)\r\n";
#endif /* } */
  va_list argptr;
  memset(buffer, '\0', sizeof(buffer));
  extendlength = strlen(formathead) + strlen(formatend);
  newformat_length = strlen(format) + extendlength; 
  newformat = (char*)malloc(sizeof(char) * newformat_length);
  if (NULL == newformat) return;
  memset(newformat, '\0', newformat_length);
  strcpy(newformat, formathead); 
  strcat(newformat, format);
  strcat(newformat, formatend);
  va_start(argptr, format);
  vsnprintf(buffer, sizeof(buffer) - 1, newformat, argptr);
  va_end(argptr);
  printf(buffer);
  if (newformat != NULL) {
    free(newformat);
    newformat = NULL;
  }
}

void baseio_pwarn(const char* format, ...) {
  char* newformat = NULL;
  uint32_t newformat_length = 0;
  uint32_t extendlength = 0;
  char buffer[2048];
#if defined(__LINUX__) /* { */
  const char* formathead = "\e[0;33;1m";
  const char* formatend = "\e[0m\n";
#elif defined(__WINDOWS__) /* }{ */
  const char* formathead = "(warning)->";
  const char* formatend = "<-(warning)\r\n";
#endif /* } */
  va_list argptr;
  memset(buffer, '\0', sizeof(buffer));
  extendlength = strlen(formathead) + strlen(formatend);
  newformat_length = strlen(format) + extendlength; 
  newformat = (char*)malloc(sizeof(char) * newformat_length);
  if (NULL == newformat) return;
  memset(newformat, '\0', newformat_length);
  strcpy(newformat, formathead); 
  strcat(newformat, format);
  strcat(newformat, formatend);
  va_start(argptr, format);
  vsnprintf(buffer, sizeof(buffer) - 1, newformat, argptr);
  va_end(argptr);
  printf(buffer);
  if (newformat != NULL) {
    free(newformat);
    newformat = NULL;
  }
}
