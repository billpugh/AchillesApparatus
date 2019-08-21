
#include <Arduino.h>
#include "Gen.h"


int generatorInputPins [] = {A4, A3, A2, A1, A0,
                             9, 10, 11, 12, 13
                            };


int ledPos[] = {1, 2, 3, 4, -1, 5, 6, 7, 8, 9, 10};



boolean rebooting;
unsigned long rebootStarted=0;

int reboot state = 0;



 
void startReboot() [
 if (rebooting) Serial.println("already rebooting")
 
}
 
 uint8_t Gen::brightness() {
  if (on) return 255;
  return 0;
}

void Gen::update() {
   bool pressed = !digitalRead(generatorInputPins[id]);
   if (pressed == buttonPressed) return;
   buttonPressed = pressed;
   changeState();
}

int Gen:: currentVoltageTarget() {
      if (on) return operatingVoltage;
      else return 0;
    }
    void Gen:: changeState() {
      voltageWhenLastChanged = currentVoltageContribution;
      stable =false;
      stateLastChanged = millis();
      on = !on;
      
    }
    int Gen:: currentVoltageContribution() {
      if (stable)
        float spread;
      if (on)
        spread = operatingVoltage + bonusVoltage - voltageWhenLastChanged;
      else
        spread = voltageWhenLastChanged - + bonusVoltage;
      unsigned long diff = millis() - stateLastChanged;
      float delta = diff / 1000.0;
      int v = (delta) / (delta + halfLife) * spread + voltageWhenLastChanged;
      if (on) {
        if (v > operatingVoltage) {
          v = operatingVoltage;
          stable = true;
        }
      } else if (v < 0) {
        v = 0;
        stable = true;
      }
      return v;
    }
