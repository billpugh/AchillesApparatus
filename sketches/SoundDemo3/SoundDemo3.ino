

// 3) Connect 3 wires from the Arduino to the Tsunami's serial connector:
//
//    Arduino       Tsunami
//    =======       =======
//    GND  <------> GND
//    TXn  <------> RX
//    RXn  <------> TX

#include <Adafruit_NeoPixel.h>
#include <Tsunami.h>
#define TSUNAMI_NUM_OUTPUTS     8

#define LED 13                  // our LED

Tsunami tsunami(Serial1);                // Our Tsunami object
int  gNumTracks;                // Number of tracks on SD card

char gTsunamiVersion[VERSION_STRING_LEN];    // Tsunami version string

const int OUTPUT_CHANNEL = 0;
// ****************************************************************************

void tsunamiDelay(int ms) {
  unsigned long done = millis() + ms;
  do {
    tsunami.update();
    delay(1);

  } while (millis() < done);

}


void tsunamiReset(Tsunami &t) {
  t.start();
  delay(10);

  int num = t.getNumTracks();
  Serial.print("num tracks: ");
  Serial.println(num);

  // Send a stop-all command and reset the sample-rate offset, in case we have
  //  reset while the Tsunami was already playing.
  t.stopAllTracks();
  for (int i = 0; i < TSUNAMI_NUM_OUTPUTS; i++) {
    t.samplerateOffset(i, 0);
    t.masterGain(i, 0);
  }
}


void setup() {

  // Serial monitor
  Serial.begin(115200);


  // We should wait for the Tsunami to finish reset before trying to send
  // commands.
  delay(1000);
  while (!Serial && millis() < 12000) delay(1);
  Serial.println("Tsunami demo");

  // Tsunami startup at 57600
  tsunamiReset(tsunami);


  // Enable track reporting from the Tsunami
  tsunami.setReporting(true);
  Serial.println("starting");

  // Allow time for the Tsunami to respond with the version string and
  //  number of tracks.
  delay(100);
  if (tsunami.getVersion(gTsunamiVersion, VERSION_STRING_LEN)) {
    Serial.print(gTsunamiVersion);
    Serial.print("\n");
    gNumTracks = tsunami.getNumTracks();
    Serial.print("Number of tracks = ");
    Serial.print(gNumTracks);
    Serial.print("\n");
  }
  else
    Serial.println("Didn't get version response");

  for (int t = 1; t <= 4; t++) {
    tsunami.trackLoop(t, true);
    tsunami.trackGain(t, -70);

    tsunami.trackPlayPoly(t, OUTPUT_CHANNEL, true);
  }

  Serial.println("All started");
  delay(500);
  tsunami.trackGain(2, -5);
  tsunami.trackGain(3, -5);
  tsunami.trackGain(4, -5);


}

#include "pinkNoise.h"
PinkNoise pink(4);


// ****************************************************************************
// This program uses a Metro timer to create a sequencer that steps through
//  states at 6 second intervals - you can change this rate above. Each state
//  Each state will demonstrate a Tsunami serial control feature.
//
//  In this example, some states wait for specific audio tracks to stop playing
//  before advancing to the next state.
int nextTrack = 1;
int lastButtonState = HIGH;
unsigned long lastPress = 0;
int hue;

int dbBoost(int dbDecrease) {
  float v = 2 - pow(10.0, dbDecrease / 20.0);
  v = 20.0 * log10(v);
  return (int) (v + 0.5);
}


const int masterV = 2;

void loop() {
  float p = pink.generate();
  int c = (int) 20 * p;
  int change = c;
  if (change > 8) change = change * 2 - 8;
  change = 8 - change;
  if (false) {
    Serial.print(p);

  }
  Serial.print(c);
  Serial.print(" ");
  Serial.println(change);
  if (change < 0) {
    tsunami.trackGain(2, masterV + change);
    tsunami.trackGain(3, masterV + dbBoost(change));
    tsunami.trackGain(4, masterV + dbBoost(change));
  } else {
    tsunami.trackGain(2, masterV + dbBoost(-change));
    tsunami.trackGain(3, masterV - change);
    tsunami.trackGain(4, masterV - change);
  }

  tsunami.update();
  delay(250);



}
