#include <Arduino.h>

#include "log.h"
#include <stdarg.h>
#include "Print.h"

void log(const char *s) {
  Serial.print(s);
}

const int maxLength = 500;
void logf( const char *fmt, ... ) {
  char buf[maxLength]; 
  va_list args;
  va_start (args, fmt );
  vsnprintf(buf, maxLength, fmt, args);
  va_end (args);
  Serial.print(buf);
}
