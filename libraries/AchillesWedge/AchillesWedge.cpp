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
unsigned long lastModeChange = 0;

uint16_t lastPacketReceived = 0;

uint16_t lastPacketSent = 0;

volatile boolean receivedMsg = false;
unsigned long lastMsgReceivedAt = 0;

SystemMode systemMode;

unsigned long timeSinceLastMessage() {
  return millis() - lastMsgReceivedAt;
}

void populateFromWidgetData() {
  unsigned long now = millis();
  fromWidgetData.packetNum++;
  fromWidgetData.packetAck = toWidgetData.packetNum;
  fromWidgetData.secondsSinceBoot = now / 1000;
  fromWidgetData.secondsSinceActivity = (now - lastLocalActivity) / 1000;
  // fromWidgetData. localSoundFileRequest = 0;
  // fromWidgetData. globalSoundFileRequest = 0;
  // fromWidgetData. pointsActivated = 0;
}

void processToWidgetData() {
  if (toWidgetData.packetAck != lastPacketSent)
    logf("Last packet sent %d, last packet acknowledged %d\n", lastPacketSent, toWidgetData.packetAck);

  //toWidgetData.secondsSinceBoot;
  //toWidgetData.secondsSinceActivity;
  if (systemMode != toWidgetData. systemMode)
    lastModeChange = millis();
  systemMode = toWidgetData. systemMode;
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
unsigned long activityReports = 0;
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
