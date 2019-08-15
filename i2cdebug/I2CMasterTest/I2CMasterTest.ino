//master
#include <Wire.h>

byte packet = 0;
byte incoming = 0;

void setup() {
  Wire.begin();
  Serial.begin(115200);
  delay(5000);
  Serial.println("Started I2C Master Test");
}

void loop() {
  Serial.print("Sending ");
  Serial.print(packet);
  Serial.print(" ... ");
  
  Wire.beginTransmission(0x72);
  Wire.write(packet);
  Wire.endTransmission ();
  
  Wire.requestFrom(0x72, 1);

  while (Wire.available()) {
    incoming = Wire.read();
  }

  if (incoming == packet) {
    Serial.println("success!");
  } else {
    Serial.print("failure! ");
    Serial.println(incoming);
  }

  packet += 1;
  
  delay(1000);
}
