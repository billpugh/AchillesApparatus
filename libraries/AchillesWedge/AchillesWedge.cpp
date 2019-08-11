#include <Arduino.h>
#include "AchillesWedge.h"
#include "AchillesLog.h"

#if defined(__arm__) && defined(CORE_TEENSY)
#include <i2c_t3.h>
typedef size_t  receive_arg_type;
#else
#include <Wire.h>
typedef int  receive_arg_type;
#endif

void receiveEvent(receive_arg_type len);
void requestEvent(void);

FromWidgetData fromWidgetData;
ToWidgetData toWidgetData;

SystemMode currentSystemMode = NOT_RECEIVED;
unsigned long lastLocalActivity;

void localActivitySeen() {
  lastLocalActivity = millis();
}
#ifdef ACHILLES_PACKET
uint16_t lastPacketReceived = 0;

uint16_t lastPacketSent = 0;
#endif

volatile boolean receivedMsg = false;
unsigned long lastMsgReceivedAt = 1;
unsigned long lastMsgReceivedSeen = 0;


volatile boolean sentMsg = false;
unsigned long lastMsgSentAt = 1;
unsigned long lastMsgSentSeen = 0;



SystemMode getSystemMode() {
  return toWidgetData.systemMode;
}

int getLightLevel() {
  return toWidgetData.lightLevel;
}
Daytime getDaytime() {
  return toWidgetData.daytime;
}

bool commOK() {
  if (!sentMsg || !receivedMsg) return false;
  unsigned long oneSecondAgo = millis() - 1000;
  if (lastMsgReceivedAt < oneSecondAgo || lastMsgReceivedSeen < oneSecondAgo) return false;
  return true;
}

void playSound(uint16_t track, bool global) {
  fromWidgetData.playThisTrack = track;
  fromWidgetData.playGlobal = global;
}

bool soundQueued() {
  return fromWidgetData.playThisTrack != 0;
}

bool newMessageReceived() {
  if (!receivedMsg) return false;
  if (lastMsgReceivedSeen != lastMsgReceivedAt) {
    lastMsgReceivedSeen = lastMsgReceivedAt;
    return true;
  }
  return false;
}

bool newMessageSent() {
  if (!sentMsg) return false;
  if (lastMsgSentSeen != lastMsgSentAt) {
    lastMsgSentSeen = lastMsgSentAt;
    return true;
  }
  return false;
}

unsigned long millisSinceLastMessageReceived() {
  return millis() - lastMsgReceivedAt;
}

inline uint8_t pointToBit(int p) {
  if (p < 0 || p > 7) {
    logf("being asked access point %d\n", p);
    return 0;
  }
  return 1 << p;
}
void setPoint(int p) {
  fromWidgetData.pointsActivated |= pointToBit(p);
}
void setPointTo(int p, bool value) {
  if (value)
    setPoint(p);
  else clearPoint(p);
}
void clearPoint(int p) {
  fromWidgetData.pointsActivated &= ~pointToBit(p);
}

bool getPoint(int p) {
  if (fromWidgetData.pointsActivated & pointToBit(p))
    return true;
  return false;
}
void clearAllPoints() {
  fromWidgetData.pointsActivated = 0;
}
void setPointVector(uint8_t bitvector) {
  fromWidgetData.pointsActivated  = bitvector;
}


// internal methods


void populateFromWidgetData() {
  unsigned long now = millis();
#ifdef ACHILLES_PACKET
  fromWidgetData.packetNum++;
  fromWidgetData.packetAck = toWidgetData.packetNum;
#endif
  fromWidgetData.secondsSinceBoot = constrain(now / 1000, 0, 0xffff);
  fromWidgetData.secondsSinceActivity = constrain((now - lastLocalActivity) / 1000, 0, 0xffff);
}

void processToWidgetData() {

#ifdef ACHILLES_PACKET
  if (toWidgetData.packetAck != lastPacketSent)
    logf("Last packet sent %d, last packet acknowledged %d\n", lastPacketSent, toWidgetData.packetAck);
#endif
  //toWidgetData.secondsSinceBoot;
  //toWidgetData.secondsSinceActivity;

}

unsigned long lastActivityAt = 0;

void setupComm(int wedgeAddress) {

#if defined(__arm__) && defined(CORE_TEENSY)
  log("setting up i2c on Teensy\n");
  // Setup for Slave mode, address 0x44, pins 18/19, external pullups, 400kHz
  Wire.begin(I2C_SLAVE, wedgeAddress, I2C_PINS_18_19, I2C_PULLUP_EXT, I2C_RATE_400);
  Wire.setDefaultTimeout(100); // 100 usecs timeout
#else
  log("setting up i2c on non-Teensy\n");
  Wire.begin(wedgeAddress);
#endif
  logf("i2c Slave address: 0x%02x\n", wedgeAddress);
  // init vars

  // register events
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

}


//
// handle Rx Event (incoming I2C request/data)
//
void receiveEvent(receive_arg_type len)
{
  if (len != sizeof(toWidgetData)) {
    logf("Received i2c msg of length %d, rather than %d\n", len, sizeof(toWidgetData));
    return;
  }

  memset(&toWidgetData, 0, sizeof(toWidgetData));

  receivedMsg = true;
  lastMsgReceivedAt = millis();
  Wire.readBytes((uint8_t*)&toWidgetData, len); // copy Rx data to databuf

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
  logf("requestEvent, writing %d bytes\n", sizeof(FromWidgetData));
  populateFromWidgetData();
  uint8_t * p = (uint8_t *)&fromWidgetData;
  Wire.write(p, sizeof(FromWidgetData));
  sentMsg = true;
  lastMsgSentAt = millis();
  fromWidgetData.playGlobal = false;
  fromWidgetData.playThisTrack = 0;

}


#if defined(__arm__) && defined(CORE_TEENSY)
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
#endif
