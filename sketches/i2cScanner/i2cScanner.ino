
#include <Wire.h>
#include "AchillesLog.h"


void setup()
{
  Wire.begin();
  Serial.begin(115200);

  while (!Serial) delay(10);            // Leonardo: wait for serial monitor
  log("\nI2C Scanner\n");
}


void loop()
{
  byte error, address;
  int nDevices;

  log("Scanning...\n");

  nDevices = 0;
  for (int address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    log(".");
    if (address % 32 == 0) log("\n");

    if (error == 0)
    {
      logf("\nI2C device found at address %02x\n", address);


      nDevices++;
    }
    else if (error != 2)
    {
      logf("\nerror %d at address %02x\n", error, address);

    }
  }
  logf("\n%d i2C devices found", nDevices);


  delay(5000);           // wait 5 seconds for next scan
}