
#include <Adafruit_NeoPixel.h>
#include <Tsunami.h>
#define TSUNAMI_NUM_OUTPUTS     8


#define LED 13                  // our LED

Tsunami tsunami(Serial1);                // Our Tsunami object
int  gNumTracks;                // Number of tracks on SD card

char gTsunamiVersion[VERSION_STRING_LEN];    // Tsunami version string

const int OUTPUT_CHANNEL = 0;


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
  for (int i = 1; i < num; i++)
    t.trackGain(i, 0);

  // Send a stop-all command and reset the sample-rate offset, in case we have
  //  reset while the Tsunami was already playing.
  t.stopAllTracks();
  for (int i = 0; i < TSUNAMI_NUM_OUTPUTS; i++) {
    t.samplerateOffset(i, 0);
    t.masterGain(i, 0);
  }
}
void setup() {
  delay(1000);
  while (!Serial && millis() < 12000) delay(1);
  Serial.println("Tsunami demo");

  tsunamiReset(tsunami);
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


}

void loop() {
  Serial.println("Starting");
  tsunami.trackLoop(6, true);
   tsunami.trackLoop(7, true);
  tsunami.trackLoad(6, OUTPUT_CHANNEL, true);         // Load and pause Track 6
  tsunami.trackLoad(7, OUTPUT_CHANNEL, true);         // Load and pause Track 7
  //tsunami.trackLoad(8, OUTPUT_CHANNEL, true);         // Load and pause Track 8
  tsunami.resumeAllInSync();             // Start all in sample sync

  // Decrement the sample rate offset from 0 to -32767 (1 octave down)
  //  in 10 ms steps
  tsunamiDelay(500);
  Serial.println("Lowering pitch");
  tsunami.samplerateOffset(OUTPUT_CHANNEL, -32767);
  tsunamiDelay(2000);
  Serial.println("Raising pitch");
  tsunami.samplerateOffset(OUTPUT_CHANNEL, 32767);
  tsunamiDelay(2000);
  Serial.println("Back to normal pitch");
  tsunami.samplerateOffset(OUTPUT_CHANNEL, 0);
  Serial.println("Holding");
  // Hold for 1 second, the stop all tracks
  tsunamiDelay(1000);
  tsunami.stopAllTracks();               // Stop all
  Serial.println("Done");

}
