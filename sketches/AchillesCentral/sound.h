

// #define SKIP_TIME SKIP_TIME

#include "Achilles.h"
#include "AchillesCentral.h"

#ifndef SOUND_H
#define SOUND_H
extern bool soundReady;

void initializeSound();

void playSound(const WedgeData &w);
void updateSound();
#endif
