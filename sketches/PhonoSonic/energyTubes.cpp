
#include <Arduino.h>
#include <Servo.h>
Servo tubeServo;
#include "AchillesLog.h"

// inner most to outer most
const uint8_t red_led_pin = 9;
const uint8_t yellow_led_pin = 10;
const uint8_t green_led_pin = 6;


class EnergyTube {
  unsigned long lastPower = 0;
};

const uint8_t servo_pin = 5;

const uint16_t minMicros = 1080;

const uint16_t maxMicros = 1550;
float twoPi = 6.2831853072;
unsigned long periodReset = 0;
float radiansAtReset;
float tubeAmplitude = 0.0;
unsigned int tubePeriod = 2000;


void initializeTubes() {
  tubeServo.attach(servo_pin, minMicros, maxMicros);
  tubeServo.writeMicroseconds(maxMicros);
  pinMode(red_led_pin, OUTPUT);
  digitalWrite(red_led_pin, LOW);
  pinMode(yellow_led_pin, OUTPUT);
  digitalWrite(yellow_led_pin, LOW);
  pinMode(green_led_pin, OUTPUT);
  digitalWrite(green_led_pin, LOW);
}

void setLEDLevels(uint8_t red, uint8_t yellow, uint8_t green);
bool tubeOff() {
  return tubeAmplitude < 0.01;
}
float getRadians(unsigned long ms) {
  radiansAtReset + (ms - periodReset)*twoPi / tubePeriod;
}
int getPulseLength(float depth) {
  return constrain((int)(maxMicros - (maxMicros - minMicros) * depth),
                   minMicros, maxMicros);
}

void setTubePeriod(unsigned int tubePeriodMS) {
  unsigned long now = millis();
  radiansAtReset = getRadians(now);
  periodReset = now;
}

void setTubeAmplitude(float amplitude) {
  tubeAmplitude = constrain(amplitude, 0, 1);
  if (tubeOff())
    tubeServo.writeMicroseconds(maxMicros);
}

//
void updateTubes() {
  if (tubeOff()) return;
  unsigned long now = millis();
  float depth = tubeAmplitude * sin(getRadians(now) + 1) / 2;
  int pulselen = getPulseLength(depth);
  tubeServo.writeMicroseconds(pulselen);
}
