
#ifndef ACHILLES_WEDGE_H
#define ACHILLES_WEDGE_H

#include "Achilles.h"

void setupComm(int wedgeAddress);
unsigned long timeSinceLastMessage();
extern unsigned long lastLocalActivity;
extern unsigned long lastGlobalActivity;

extern volatile boolean receivedMsg;
extern unsigned long lastMsg;

#endif
