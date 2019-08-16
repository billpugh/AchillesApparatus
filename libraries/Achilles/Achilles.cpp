
#include <Arduino.h>
#include "Achilles.h"


const char * soundActionName(SoundAction s) {
  switch (s) {
    case PLAY : return "Play";
    case STOP : return "Stop";
    case FADE_OUT_200MS: return "Fade out 200 ms";
    case FADE_OUT_1000MS: return "Fade out 1000 ms";
    default: return "Unknown Sound Action";
  }
}

