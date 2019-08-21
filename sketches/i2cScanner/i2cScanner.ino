
#include <Wire.h>
#include "AchillesLog.h"


void setup()
{
  Wire.begin();
  Serial.begin(115200);

  while (!Serial) delay(10);           
  aalog("\nI2C Scanner\n");
}


void loop()
{
  byte error, address;
  int nDevices;

  aalog("Scanning...\n");

  nDevices = 0;
  for (int address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    aalog(".");
    if (address % 32 == 0) log("\n");

    if (error == 0)
    {
      aalogf("\nI2C device found at address %02x\n", address);


      nDevices++;
    }
    else if (error != 2)
    {
      aalogf("\nerror %d at address %02x\n", error, address);

    }
  }
  aalogf("\n%d i2C devices found", nDevices);


  delay(5000);           // wait 5 seconds for next scan
}
