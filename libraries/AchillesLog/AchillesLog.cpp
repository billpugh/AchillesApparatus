#include <Arduino.h>

#include "AchillesLog.h"
#include <stdarg.h>
#include "Print.h"

void aalog(const char *s) {
  Serial.print(s);
}

const int maxLength = 500;
void aalogf( const char *fmt, ... ) {
  char buf[maxLength]; 
  va_list args;
  va_start (args, fmt );
  vsnprintf(buf, maxLength, fmt, args);
  va_end (args);
  Serial.print(buf);
}
