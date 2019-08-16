
#define SKIP_TIME SKIP_TIME

#ifndef SKIP_TIME
#include "RTClib.h"

extern DateTime playaTime;

#endif
extern bool clockRunning;
void initializeClock();
void updateClock();
