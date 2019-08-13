#include <Arduino.h>

#include <Tsunami.h>
#include "pinkNoise.h"
#include "AchillesLog.h"
#include "AchillesCentral.h"

Tsunami panelTsunami(Serial1);
//Tsunami mainTsunami(Serial2);


bool soundReady = false;
int lastTrackPlayedOnPanel[TSUNAMI_NUM_OUTPUTS];

int lastTrackPlayedOnMain[TSUNAMI_NUM_OUTPUTS];

void tsunamiReset(Tsunami &t) {
  Serial.println("Resetting Tsunami");
  t.start();
  delay(10);

  int num = t.getNumTracks();
  logf("num tracks: %d\n", num);
  for (int i = 0; i < TSUNAMI_NUM_OUTPUTS; i++) {
    t.samplerateOffset(i, 0);
    t.masterGain(i, 0);
  }

  // Send a stop-all command and reset the sample-rate offset, in case we have
  //  reset while the Tsunami was already playing.
  t.stopAllTracks();

}

void initializeSound() {
  soundReady = true;
  for (int i = 0; i < TSUNAMI_NUM_OUTPUTS; i++) {
    lastTrackPlayedOnPanel[i] = 0;
    lastTrackPlayedOnMain[i] = 0;
  }


  tsunamiReset(panelTsunami);
  // tsunamiReset(mainTsunami);
}

int getFileNum(int track, int wedgeId) {
  return wedgeId * 100 + track;
}

void resetTrack(Tsunami &t, int track) {
  t.trackGain(track, 0);
}


void updateSound() {

}


void playSound(const WedgeData & w) {
  if (w.position < 0 || w.data.playThisTrack == 0)
    return;

  int track = getFileNum(w.data.playThisTrack, w.id);

  if (!w.data.playGlobal) {
    logf("playing track %d on position %d\n", track, w.position);

    if (!soundReady) return;
    resetTrack(panelTsunami,  track);
    panelTsunami.trackPlayPoly(track, w.position, true);
    lastTrackPlayedOnPanel[w.position] = track;
  } else {
    logf("playing track %d on global %d\n", track);

    if (!soundReady) return;
    //    int speaker = 0;
    //    resetTrack(mainTsunami,  track);
    //    mainTsunami.trackPlayPoly(track, speaker, true);
    //    lastTrackPlayedOnPanel[speaker] = track;
  }

}
