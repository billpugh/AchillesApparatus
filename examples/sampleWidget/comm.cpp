#include <Arduino.h>
#include "comm.h"
#include "log.h"

#if defined(__arm__) && defined(CORE_TEENSY)

#include <i2c_t3.h>
#else
#include <Wire.h>
#endif

const uint8_t slaveAddress =  0x44;

const uint8_t rebootStarted = 255;
const uint8_t rebootActivity = 254;
const uint8_t slaveRebootStarted = 253;

void receiveEvent(size_t len);
void requestEvent(void);

volatile boolean receivedMsg = false;
unsigned long lastMsg = 0;

extern unsigned long timeSinceLastMessage() {
  return millis() - lastMsg;
}

unsigned long lastActivityAt = 0;

struct __attribute__ ((packed)) ActivityData {
  uint16_t secondsSinceLastActivity;
  uint16_t secondsSinceBoot;
  uint8_t lastActivity = slaveRebootStarted;
  uint8_t subActivity = 0;
  uint8_t reboots = 0;
};

ActivityData activityData;

void setupComm() {

#if defined(__arm__) && defined(CORE_TEENSY)
  log("setting up i2c on Teensy");
  // Setup for Slave mode, address 0x44, pins 18/19, external pullups, 400kHz
  Wire.begin(I2C_SLAVE, slaveAddress, I2C_PINS_18_19, I2C_PULLUP_EXT, I2C_RATE_400);
  Wire.setDefaultTimeout(100); // 100 usecs timeout
#else
log("setting up i2c on non-Teensy");
 Wire.begin(slaveAddress);
#endif
  logf("i2c Slave address: 0x%02x\n", slaveAddress);
  // init vars

  // register events
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

}
unsigned long activityReports = 0;
//
// handle Rx Event (incoming I2C request/data)
//
void receiveEvent(size_t len)
{
  uint8_t kind = Wire.read();
  if (kind == 42) {
   
   logf("Got unknown i2c message, kind %d, %d bytes\n",
             kind, Wire.available());
  }
}


uint16_t secondsBetween(unsigned long start, unsigned long end) {
  unsigned long diff = (end - start) / 1000;
  if (diff > 65535) return 65535;
  return diff;
}
//
// handle Tx Event (outgoing I2C data)
//
void requestEvent(void)
{
  Serial.println("requestEvent...");
  unsigned long now = millis();
  activityData.secondsSinceLastActivity = secondsBetween(lastActivityAt, now);
  activityData.secondsSinceBoot = secondsBetween(0, now);
  uint8_t * p = (uint8_t *)&activityData;
  Wire.write(p, sizeof(ActivityData));
}


void print_i2c_status(void)
{
  switch (Wire.status())
  {
    case I2C_WAITING:
      //Serial.print("I2C waiting, no errors\n");
      break;
    case I2C_ADDR_NAK: Serial.print("Slave addr not acknowledged\n"); break;
    case I2C_DATA_NAK: Serial.print("Slave data not acknowledged\n"); break;
    case I2C_ARB_LOST: Serial.print("Bus Error: Arbitration Lost\n"); break;
    default:           Serial.print("I2C busy\n"); break;
  }
}
