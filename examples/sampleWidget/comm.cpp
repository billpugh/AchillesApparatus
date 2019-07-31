#include <Arduino.h>
#include "comm.h"
#include "log.h"

#if defined(__arm__) && defined(CORE_TEENSY)

#include <i2c_t3.h>
#else
#include <Wire.h>
#endif

const uint8_t slaveAddress =  0x44;

const uint8_t packetKind = 0xaa;

const uint8_t rebootStarted = 255;
const uint8_t rebootActivity = 254;
const uint8_t slaveRebootStarted = 253;



void receiveEvent(size_t len);
void requestEvent(void);


FromWidgetData fromWidgetData;
ToWidgetData toWidgetData;

SystemMode currentSystemMode = NOT_RECEIVED;
unsigned long lastModeChange = 0;

uint16_t lastPacketReceived = 0;

uint16_t lastPacketSent = 0;

volatile boolean receivedMsg = false;
unsigned long lastMsg = 0;

SystemMode systemMode;

extern unsigned long timeSinceLastMessage() {
  return millis() - lastMsg;
}

void populateFromWidgetData() {
  unsigned long now = millis();
  fromWidgetData.packetNum = lastPacketSent++;
  fromWidgetData. packetAck = lastPacketReceived;
  fromWidgetData.secondsSinceBoot = now / 1000;
  fromWidgetData.secondsSinceActivity = (now - lastLocalActivity) / 1000;
  // fromWidgetData. localSoundFileRequest = 0;
  // fromWidgetData. globalSoundFileRequest = 0;
  // fromWidgetData. pointsActivated = 0;
}
void processToWidgetData() {
  lastPacketReceived = toWidgetData.packetNum;
  if (toWidgetData.packetAck != lastPacketSent)
    logf("Last packet sent %d, last packet acknowledged %d\n", lastPacketSent, toWidgetData.packetAck);

  //toWidgetData.secondsSinceBoot;
  //toWidgetData.secondsSinceActivity;
  if (systemMode != toWidgetData. systemMode)
    lastModeChange = millis();
  systemMode = toWidgetData. systemMode;
}

unsigned long lastActivityAt = 0;

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
  if (len == 0) {
    log("Received i2c msg of length 0");
    return;
  }
  uint8_t kind = Wire.read();
  switch (kind) {
    case packetKind:
      memset(&toWidgetData, 0, sizeof(toWidgetData));

      if (len - 1 != sizeof(toWidgetData)) {
      logf("Received data, but %d bytes rather than the expected %d bytes\n",
           len - 1, toWidgetData);
      }
      Wire.read((uint8_t*)&toWidgetData, len - 1); // copy Rx data to databuf
      break;
    default:


        logf("Got unknown i2c message, kind %d, %d bytes\n",
             kind, len - 1);
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
  populateFromWidgetData();
  uint8_t * p = (uint8_t *)&fromWidgetData;
  Wire.write(p, sizeof(FromWidgetData));
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
