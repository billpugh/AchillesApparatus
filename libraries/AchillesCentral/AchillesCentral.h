
#ifndef ACHILLES_CENTRAL_H
#define ACHILLES_CENTRAL_H

#include "Achilles.h"



class WedgeData {
    public:
    const char* name;
    const int address;
    const int position;
    unsigned long lastBoot;
    unsigned long lastActivity;
    uint8_t points;
    FromWidgetData data;
    bool responsive = true;

    WedgeData(const char * name, int address, int position) : name(name),
      address (address), position(position) {};
};

extern WedgeData wedges[];
extern const size_t numWedges;
extern ToWidgetData centralData;
extern int totalPoints;

extern unsigned long lastActivity;

void initializeCentral();
void scanWedges(SystemMode systemMode);
int totalPoints();




#endif
