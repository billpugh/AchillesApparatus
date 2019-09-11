

#include <Adafruit_NeoPixel.h>
#include <Tsunami.h>
#define TSUNAMI_NUM_OUTPUTS     8

const int ambientSounds = 17;
const int momentSounds = 15;
const int laughSounds = 5;

const int ambientGroup = 0;
const int momentGroup = 100;
const int laughGroup = 200;

unsigned long nextAmbient = 0;

unsigned long nextMoment = 5000;
unsigned long nextLaugh = 15000;


const int ambientTime = 60000;
const int momentTime = 47000;

const int laughTime = 5 * 60 * 1000;

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


void demoAll() {
  Serial.println("Ambient");
  for (int i = 1; i <= ambientSounds; i++) {
    int track = ambientGroup + i;
    Serial.println(track);
    tsunami.trackGain(track, 0);
    tsunami.trackPlayPoly(track, OUTPUT_CHANNEL, true);
    delay(1000);
    tsunami.trackStop(track);
  }
  Serial.println("Moment");
  for (int i = 1; i <= momentSounds; i++) {
    int track = momentGroup + i;
    Serial.println(track);
    tsunami.trackGain(track, 0);
    tsunami.trackPlayPoly(track, OUTPUT_CHANNEL, true);
    delay(1000);
    tsunami.trackStop(track);
  }
  Serial.println("Laugh");
  for (int i = 1; i <= laughSounds; i++) {
    int track = laughGroup + i;
    Serial.println(track);
    tsunami.trackGain(track, 0);
    tsunami.trackPlayPoly(track, OUTPUT_CHANNEL, true);
    delay(1000);
    tsunami.trackStop(track);
  }
}

void setup() {

  // Serial monitor
  Serial.begin(115200);


  // We should wait for the Tsunami to finish reset before trying to send
  // commands.
  delay(500);
  Serial.println("Tsunami demo");
  random(123);

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
  //demoAll();

}
int nextTrack(int top, int last) {
  int r = random(top) + 1;
  while (r == last)
    r = random(top) + 1;
  return r;
}
int lastAmbient = -1;
int lastMoment = -1;
int lastLaugh = -1;

void loop() {

  unsigned long now = millis();

  if (nextAmbient < now) {
    if (lastAmbient > 0)
      tsunami.trackFade(lastAmbient, -40, 1000, true);
    int track = ambientGroup + nextTrack(ambientSounds, lastAmbient);
    Serial.print("Now ");
    Serial.println(now);
    Serial.print("Playing ambient ");
    Serial.println(track);
    tsunami.trackLoop(track, true);
    tsunami.trackGain(track, -40);
    tsunami.trackPlayPoly(track, OUTPUT_CHANNEL, true);
    tsunami.trackFade(track, 0, 1000, false);
    lastAmbient = track;
    nextAmbient += ambientTime + random(5000);
    Serial.print("Next: ");
    Serial.println(nextAmbient);
  }

  if (nextMoment < now) {
    if (lastMoment > 0)
      tsunami.trackFade(lastMoment, -40, 500, true);
    Serial.print("Now ");
    Serial.println(now);
    int track = momentGroup + nextTrack(momentSounds, lastMoment);
    Serial.print("Playing moment ");
    Serial.println(track);
    tsunami.trackLoop(track, false);
    tsunami.trackGain(track, -40);
    tsunami.trackPlayPoly(track, OUTPUT_CHANNEL, false);
    tsunami.trackFade(track, 0, 500, false);
    lastMoment = track;
    nextMoment += momentTime + random(5000);
    Serial.print("Next: ");
    Serial.println(nextMoment);
  }
  if (nextLaugh < now) {
    if (lastLaugh > 0)
      tsunami.trackFade(lastLaugh, -40, 500, true);
    Serial.print("Now ");
    Serial.println(now);
    int track = laughGroup + nextTrack(laughSounds, lastLaugh);
    Serial.print("Playing Laugh ");
    Serial.println(track);
    tsunami.trackLoop(track, false);
    tsunami.trackGain(track, -40);
    tsunami.trackPlayPoly(track, OUTPUT_CHANNEL, false);
    tsunami.trackFade(track, 0, 100, false);
    lastLaugh = track;
    nextLaugh += laughTime + random(5000);
    Serial.print("Next: ");
    Serial.println(nextLaugh);
  }



}
