//master
#include <Wire.h>

void setup() {
  Wire.begin();
  Serial.begin(115200);
  delay(5000);
  Serial.println("Started I2C Master Test");
}

void loop() {
  Serial.print("Sending 7 bytes... ");
  
  Wire.beginTransmission(0x72);
  Wire.write("0123456");
  Wire.endTransmission ();

  Serial.println("Requesting 9 bytes... ");
  Wire.requestFrom(0x72, 9);

  Serial.print("Incoming '");
  byte count = 0;
  while (Wire.available()) {
    Serial.print((char) Wire.read());
    ++count;
  }
  Serial.print("' ");
  Serial.print(count);
  Serial.println(" bytes");

  delay(1000);
}
