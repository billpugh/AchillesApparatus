
#include <Adafruit_NeoPixel_ZeroDMA.h>

Adafruit_NeoPixel_ZeroDMA strip(1, 1, NEO_GRB);
const int red_led = 10;
const int yellow_led = 9;
const int green_led = 6;
const int servo_pin = 5;

#include <Servo.h>
#include "AchillesLog.h"
Servo myservo;

void setup() {
  Serial.begin(115200);
  while (!Serial ) delay(1);
  logf("PhotoSonic, compiled %s, %s\n",
       F(__DATE__), F(__TIME__));
  strip.begin();
  myservo.attach(servo_pin, 1000, 2000);
  pinMode(red_led, OUTPUT);
  pinMode(yellow_led, OUTPUT);
  pinMode(green_led, OUTPUT);
  pinMode(green_led, OUTPUT);
  pinMode(13, OUTPUT);

}

void loop() {
  Serial.println(millis());
  digitalWrite(red_led, HIGH);
  digitalWrite(13, HIGH);
  delay(200);
  digitalWrite(yellow_led, HIGH);
  delay(200);
  digitalWrite(red_led, LOW);
  digitalWrite(13, LOW);
  digitalWrite(green_led, HIGH);
  delay(200);

  digitalWrite(yellow_led, LOW);
  delay(200);
  digitalWrite(green_led, LOW);
  delay(200);
  // put your main code here, to run repeatedly:

}
