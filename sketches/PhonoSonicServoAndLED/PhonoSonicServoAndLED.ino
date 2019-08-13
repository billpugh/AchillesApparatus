#include <Adafruit_NeoPixel_ZeroDMA.h>

Adafruit_NeoPixel_ZeroDMA strip(1, 23, NEO_GRB);

#include <Servo.h>
Servo myServo;


const uint16_t minMicros = 1080;

const uint16_t maxMicros = 1550;
const uint16_t lowMicros = minMicros
                           + (maxMicros - minMicros) / 3;
const uint16_t highMicros = minMicros
                            + 2 * (maxMicros - minMicros) / 3;


void setup() {
  myServo.attach(5, minMicros, maxMicros);
  myServo.writeMicroseconds(1500);
  Serial.begin(115200);
  while (!Serial) delay(10);
  Serial.println("Starting");
  strip.begin();
  strip.show();

}


int millisecondsPerWave = 2000;
float amplitude = 1.0;
float twoPi = 6.2831853072;

// depth should be 0...1, with 1 being maximum depth
int getPulseLength(float depth) {
  return (int)(maxMicros - (maxMicros-minMicros)*depth);
}

  const uint8_t red_led_pin = 9;
const uint8_t yellow_led_pin = 10;
const uint8_t green_led_pin = 6;

void loop() {
  
  float radians = ((float)millis() / millisecondsPerWave) * twoPi;
  float depth = (sin(radians)+1)*amplitude/2;
  int pulselen = constrain(getPulseLength(depth), minMicros, maxMicros);
  Serial.print(pulselen);
  Serial.print(" ");
  Serial.println((int)(255*depth));
  myServo.writeMicroseconds(pulselen);
  analogWrite(red_led_pin, 255*depth);
analogWrite(green_led_pin, 255-255*depth);

  strip.setPixelColor(0, 255*depth, 0, 0);
  strip.show();
  delay(20);
}

//void loop() {
//  return;
//  Serial.println(millis());
//  myServo.writeMicroseconds(maxMicros);
//  strip.setPixelColor(0, 255, 0, 0);
//  strip.show();
//  delay(500);
//  myServo.writeMicroseconds(highMicros);
//
//  strip.setPixelColor(0, 0, 255, 0);
//  strip.show();
//  delay(500);
//  myServo.writeMicroseconds(lowMicros);
//  strip.setPixelColor(0, 0, 0, 255);
//  strip.show();
//  delay(500);
//  myServo.writeMicroseconds(minMicros);
//
//  strip.setPixelColor(0, 0, 0, 0);
//  strip.show();
//  delay(500);
//
//}
