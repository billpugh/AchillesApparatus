
void setupComm();
extern unsigned long timeSinceLastMessage();
extern volatile boolean receivedMsg;
extern unsigned long lastMsg;

struct __attribute__ ((packed)) FromWidgetData {
  uint16_t secondsSinceBoot;
  uint16_t secondsSinceActivity;
  /** # of a sound file that should be played on the wedge speaker */
  uint16_t localSoundFileRequest = 0;
  /** # of a sound file that should be played on the big shared speakers */
  uint16_t globalSoundFileRequest = 0;

  /** Each wedge has 8 points/levels that can be activated/won; set the bits 
   *  for the points that have been won.
   */
  uint8_t pointsActivated = 0;
};


/** Overall Achilles Apparatus modes:
 *  QUIET - No recent activity
 *  ACTIVE - Someone is visiting the Achilles Apparatus
 *  CHARGED - The Achilles Apparatus is fully charged
 *  DISCHARGING - They reached the end goal, and allowed a spirit reach Metamorphosis.
 *  RESET - After discharge, all puzzles and completity levels should be reset and puzzles should 
 *  not be active. The reset state will persist for several seconds, perhaps even a full minute, encouaging
 *  the people who had been playing to take a break. 
 *  CHEAT_CODE - possible cheat code mode, in which the entire system becomes more 
 *  of a sound board
 */
enum SystemMode { QUIET, ACTIVE, CHARGED, DISCHARGING, RESET,  CHEAT_CODE };

struct __attribute__ ((packed)) ToWidgetData {
  uint16_t secondsSinceBoot;
  uint16_t secondsSinceActivity;
  SystemMode systemMode;
};
