
#ifndef ACHILLES_CENTRAL_H
#define ACHILLES_CENTRAL_H

#include "Achilles.h"



class WedgeData {
    public:
    const char* name;
    const int address;
    const int position;
    const int id;
    unsigned long lastBoot;
    unsigned long lastActivity;
    uint8_t points;
    FromWidgetData data;
    bool responsive = true;

    WedgeData(const char * name, int address, int position) : name(name),
      address (address), position(position), id(position) {};
       WedgeData(const char * name, int address, int position, int id) : name(name),
      address (address), position(position), id(id) {};
};

extern WedgeData wedges[];
extern const size_t numWedges;
extern ToWidgetData centralData;



struct __attribute__ ((packed)) ToLEDTowerData {
  uint16_t packetNum = 0;
  uint16_t minutesSinceGlobalBoot;
  uint16_t secondsSinceGlobalActivity;
  SystemMode systemMode = NOT_RECEIVED;
  Daytime daytime = UNKNOWN_SUNLIGHT;
  int8_t lightLevel = -2; // -4 = nighttime, 0 = daytime
};

extern ToLEDTowerData towerData;

extern unsigned long lastActivity;

void initializeCentral();
void scanWedges(SystemMode systemMode);
void updateCentralData(SystemMode systemMode);
extern int totalPoints;




#endif
