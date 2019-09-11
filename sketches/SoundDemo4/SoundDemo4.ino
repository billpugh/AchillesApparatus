

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

const int OUTPUT_CHANNEL = 7;
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
  tsunami.start();
  delay(10);


  // Enable track reporting from the Tsunami
  tsunami.setReporting(true);
  tsunamiDelay(200);


  Serial.println("starting");
  // Allow time for the Tsunami to respond with the version string and
  //  number of tracks.
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

  Serial.println("starting track 6");
  tsunami.trackGain(6, 0);
  tsunami.trackLoop(6, false);
  tsunami.trackPlayPoly(6, 7, true);
  Serial.println("track 6 started");
  tsunami.trackFade(6,-60,10000,true);


}


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

void loop() {

  tsunami.update();
  delay(10);



}
