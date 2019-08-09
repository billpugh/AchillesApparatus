
#ifndef ACHILLES_CENTRAL_H
#define ACHILLES_CENTRAL_H

#include "Achilles.h"

class WedgeData {
  const char* name;
  const int wedgeAddress;
  const int wedgePosition;
  unsigned long lastBoot;
  unsigned long lastActivity;
  uint8_t points; 
  public:
  WedgeData(const char * name, int address, int position) : name(name), 
  wedgeAddress (address), wedgePosition(position) {};
};


  
  


#endif
