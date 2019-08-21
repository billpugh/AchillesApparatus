

#include "leds.h"
#include "AchillesLog.h"

void setup() {

 
  Serial.begin(115200);
  while (!Serial & millis() < 3000) delay(10);
  Serial.println("starting");
  setupLEDs();
   pinMode(5, OUTPUT);
  delay(1000);

}


void setAnalogMeter(int volts) {
  int v = volts * 255/ 3080;
  int v2 = constrain(v, 0, 255);
  analogWrite(5, v2);
}

int voltage = 0;
void loop() {
  voltage+=15;
  setMeter(voltage);
  setAnalogMeter(voltage);
  

  delay(50);
  if (voltage> 3000) {
     voltage = 0;
    setAnalogMeter(0);
    showErr();
   
  }
  

}
