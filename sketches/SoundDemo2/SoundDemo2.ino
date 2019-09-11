

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
  for(int i = 0; i < TSUNAMI_NUM_OUTPUTS; i++) {
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
  tsunami.trackGain(11, 0);
  tsunami.trackLoop(11, true);
  //tsunami.trackPlayPoly(11, 6, true);
  tsunami.trackGain(12, 0);
  tsunami.trackLoop(12, true);
  //tsunami.trackPlayPoly(12, 7, true);
  tsunami.trackPlayPoly(15, 7, true);
  tsunamiDelay(1000);

  Serial.println("Starting delayed track 15");
  tsunami.trackPlayPoly(15, 7, true);
  tsunamiDelay(3000);
  Serial.println("Stopping track 15");
  tsunami.trackStop(15);
  tsunami.trackPlayPoly(16, 7, true);
  tsunamiDelay(200);
  for (int i = -30000; i < 30000; i += 500) {
    Serial.println(i);
   
    tsunami.samplerateOffset(7, i);
    tsunamiDelay(200);
  }
  tsunami.trackStop(16);

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
