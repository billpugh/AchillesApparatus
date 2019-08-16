

#ifndef ACHILLES_H
#define ACHILLES_H

//#define ACHILLES_PACKET_DEBUG ACHILLES_PACKET_DEBUG


enum SoundAction { PLAY, STOP, FADE_OUT_200MS, FADE_OUT_1000MS };


struct __attribute__ ((packed)) FromWidgetData {
#ifdef ACHILLES_PACKET_DEBUG
  uint16_t packetNum = 0;
  uint16_t packetAck;
#endif
  uint16_t secondsSinceBoot;
  uint16_t secondsSinceActivity;

  // If non-zero, request that this track be played 
  uint16_t playThisTrack = 0;
  bool playGlobal = false;
  SoundAction soundAction = PLAY; 

  /** Each wedge has 8 points/levels that can be activated/won; set the bits
      for the points that have been won.
  */
  uint8_t pointsActivated = 0;
};


/** Overall Achilles Apparatus modes:
    QUIET - No recent activity
    ACTIVE - Someone is visiting the Achilles Apparatus
    CHARGED - The Achilles Apparatus is fully charged
    DISCHARGING - They reached the end goal, and allowed a spirit reach Metamorphosis.
    RESET - After discharge, all puzzles and completity levels should be reset and puzzles should
    not be active. The reset state will persist for several seconds, perhaps even a full minute, encouaging
    the people who had been playing to take a break.
    CHEAT_CODE - possible cheat code mode, in which the entire system becomes more
    of a sound board
    NOT_RECEIVED - No communication has been received from Central
*/
enum SystemMode { QUIET, ACTIVE, CHARGED, DISCHARGING, RESET,  CHEAT_CODE, NOT_RECEIVED};


inline const char* systemModeName(SystemMode s) {
  switch (s) {
    case QUIET: return "quiet";
    case ACTIVE: return "active";
    case CHARGED: return "charged";
    case DISCHARGING: return "dischargin";
    case RESET: return "reset";
    case CHEAT_CODE: return "cheat code";
    case NOT_RECEIVED: return "not received";
    default: return "unknown";
  }
}
// For Burning Man 2019, these times start at:
// MORNING_TWILIGHT: 5:50am
// DAY: 6:20am
// EVENING_TWILIGHT: 7:38pm
// NIGHT: 8:08pm

enum Daytime { NIGHT, MORNING_TWILIGHT, DAWN, DAY, DUSK, EVENING_TWILIGHT, UNKNOWN_SUNLIGHT };

inline const char* daytimeName(Daytime d) {
  switch (d) {
    case NIGHT: return "night";
    case MORNING_TWILIGHT: return "morning twilight";
    case DAWN: return "dawn";
    case DAY: return "day";
    case DUSK: return "dusk";
    case EVENING_TWILIGHT: return "evening sunlight";
    default: return "unknown";
  }
}
// light levels: Night: -4
// Nautical Twilight: -3
// Civil Twilight: -2
// Dusk/Dawn: -1
// Daytime: 0

struct __attribute__ ((packed)) ToWidgetData {
#ifdef  ACHILLES_PACKET_DEBUG
  uint16_t packetNum = 0;
  uint16_t packetAck;
#endif
  uint16_t minutesSinceGlobalBoot;
  uint16_t secondsSinceGlobalActivity;
  SystemMode systemMode = NOT_RECEIVED;
  Daytime daytime = UNKNOWN_SUNLIGHT;
  int8_t lightLevel = -2; // -4 = nighttime, 0 = daytime
};

#endif
