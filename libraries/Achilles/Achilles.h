

#ifndef ACHILLES_H
#define ACHILLES_H

/**
 * Sound request details:
 * combining with previous sound
 * repitions
 * duration loop
 * fade out all sound
 * 
 */



enum SoundChangeMode { SOUND_COMBINE, SOUND_REPLACE, SOUND_FADE};

struct __attribute__ ((packed)) FromWidgetData {
  uint16_t packetNum;
  uint16_t packetAck;
  uint16_t secondsSinceBoot;
  uint16_t secondsSinceActivity;
  // Local sound
  /** # of a sound file that should be played on the wedge speaker */
  SoundChangeMode localSoundMode;
  // positive number is # of repeats, negative is looping duration
  int8_t localSoundDuration;
  // Number of file for this request; 0 is silence (duration ignored)
  uint16_t localSoundFile= 0;

  // Global sound
  /** # of a sound file that should be played a main speaker */
  SoundChangeMode globalSoundMode;
  // positive number is # of repeats, negative is looping duration
  int8_t globalSoundDuration;
  // Number of file for this request; 0 is silence (duration ignored)
  uint16_t globalSoundFile= 0;


  /** # of a sound file that should be played on the big shared speakers */
  uint16_t globalSoundFileRequest = 0;

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


// For Burning Man 2019, these times start at:
// MORNING_TWILIGHT: 5:50am
// DAY: 6:20am
// EVENING_TWILIGHT: 7:38pm
// NIGHT: 8:08pm

enum Sunlight { NIGHT, MORNING_TWILIGHT, DAWN, DAY, DUSK, EVENING_TWILIGHT, UNKNOWN };
// light levels: Night: -4
// Twilight: -2
// Dusk/Dawn: -1
// Daytime: 0

struct __attribute__ ((packed)) ToWidgetData {
  uint16_t packetNum;
  uint16_t packetAck;
  uint16_t secondsSinceGlobalBoot;
  uint16_t secondsSinceGlobalActivity;
  SystemMode systemMode;
  Sunlight sunlight;
  int lightLevel; // -4 = nighttime, 0 = daytime
  uint8_t hours; // 24 hour clock
  uint8_t minutes;
  uint8_t seconds;
};

#endif
