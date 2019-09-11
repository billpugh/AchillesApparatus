
#include <Adafruit_NeoPixel.h>
#include <Metro.h>              // Include the Metro library header
#define TSUNAMI_NUM_OUTPUTS     8


// ==================================================================
// The following defines are used to control which serial class is
//  used. Uncomment only the one you wish to use. If all of them are
//  commented out, the library will use Hardware Serial
#define __TSUNAMI_USE_SERIAL1__
//#define __TSUNAMI_USE_SERIAL2__
//#define __TSUNAMI_USE_SERIAL3__
//#define __TSUNAMI_USE_ALTSOFTSERIAL__
// ==================================================================

#define CMD_GET_VERSION                         1
#define CMD_GET_SYS_INFO                        2
#define CMD_TRACK_CONTROL                       3
#define CMD_STOP_ALL                            4
#define CMD_MASTER_VOLUME                       5
#define CMD_TRACK_VOLUME                        8
#define CMD_TRACK_FADE                          10
#define CMD_RESUME_ALL_SYNC                     11
#define CMD_SAMPLERATE_OFFSET           12
#define CMD_SET_REPORTING                       13
#define CMD_SET_TRIGGER_BANK            14
#define CMD_SET_INPUT_MIX                       15
#define CMD_SET_MIDI_BANK                       16

#define TRK_PLAY_SOLO                           0
#define TRK_PLAY_POLY                           1
#define TRK_PAUSE                                       2
#define TRK_RESUME                                      3
#define TRK_STOP                                        4
#define TRK_LOOP_ON                                     5
#define TRK_LOOP_OFF                            6
#define TRK_LOAD                                        7

#define RSP_VERSION_STRING                      129
#define RSP_SYSTEM_INFO                         130
#define RSP_STATUS                                      131
#define RSP_TRACK_REPORT                        132

#define MAX_MESSAGE_LEN                         32
#define MAX_NUM_VOICES                          18
#define VERSION_STRING_LEN                      22

#define SOM1    0xf0
#define SOM2    0xaa
#define EOM             0x55


#define IMIX_OUT1       0x01
#define IMIX_OUT2       0x02
#define IMIX_OUT3       0x04
#define IMIX_OUT4       0x08

#include <HardwareSerial.h>
#define TsunamiSerial Serial1

class Tsunami
{
  public:
    Tsunami() {
      ;
    }
    ~Tsunami() {
      ;
    }
    void start(void);
    void update(void);
    void flush(void);
    void setReporting(bool enable);
    bool getVersion(char *pDst, int len);
    int getNumTracks(void);
    bool isTrackPlaying(int trk);
    void masterGain(int out, int gain);
    void stopAllTracks(void);
    void resumeAllInSync(void);
    void trackPlaySolo(int trk, int out, bool lock);
    void trackPlayPoly(int trk, int out, bool lock);
    void trackLoad(int trk, int out, bool lock);
    void trackStop(int trk);
    void trackPause(int trk);
    void trackResume(int trk);
    void trackLoop(int trk, bool enable);
    void trackGain(int trk, int gain);
    void trackFade(int trk, int gain, int time, bool stopFlag);
    void samplerateOffset(int out, int offset);
    void setTriggerBank(int bank);
    void setInputMix(int mix);
    void setMidiBank(int bank);

  private:
    void trackControl(int trk, int code, int out, int flags);

#ifdef __TSUNAMI_USE_ALTSOFTSERIAL__
    AltSoftSerial TsunamiSerial;
#endif

    uint16_t voiceTable[MAX_NUM_VOICES];
    unsigned long started[MAX_NUM_VOICES];
    uint8_t rxMessage[MAX_MESSAGE_LEN];
    char version[VERSION_STRING_LEN];
    uint16_t numTracks;
    uint8_t numVoices;
    uint8_t rxCount;
    uint8_t rxLen;
    bool rxMsgReady;
    bool versionRcvd;
    bool sysinfoRcvd;
};


#define LED 13                  // our LED

Tsunami tsunami;                // Our Tsunami object
int  gNumTracks;                // Number of tracks on SD card

char gTsunamiVersion[VERSION_STRING_LEN];    // Tsunami version string

const int OUTPUT_CHANNEL = 7;
int rxTotal = 0;
// ****************************************************************************

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

  int num = tsunami.getNumTracks();
  Serial.print("num tracks: ");
  Serial.println(num);

  // Send a stop-all command and reset the sample-rate offset, in case we have
  //  reset while the Tsunami was already playing.
  tsunami.stopAllTracks();
  tsunami.samplerateOffset(0, 0);

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
    Serial.println("WAV Trigger response not available");
  tsunami.samplerateOffset(0, 0);        // Reset sample rate offset to 0
  tsunami.masterGain(6, 0);              // Reset the master gain to 0dB
  tsunami.masterGain(7, 0);              // Reset the master gain to 0dB
  tsunami.trackGain(11, 0);
  tsunami.trackLoop(11, true);
  //tsunami.trackPlayPoly(11, 6, true);
  tsunami.trackGain(12, 0);
  tsunami.trackLoop(12, true);
  //tsunami.trackPlayPoly(12, 7, true);
  tsunami.trackPlayPoly(15, 7, true);
  for (int i = 0; i <100; i++) {
    tsunami.update();
    delay(10);
  }
  Serial.println("Starting delayed track 15");
  tsunami.trackPlayPoly(15, 7, true);
  for (int i = 0; i <100; i++) {
    tsunami.update();
    delay(10);
  }
  Serial.println("Stopping track 15");
   tsunami.trackStop(15);

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


// **************************************************************
void Tsunami::start(void) {

  uint8_t txbuf[5];

  versionRcvd = false;
  sysinfoRcvd = false;
  TsunamiSerial.begin(57600);
  flush();

  // Request version string
  txbuf[0] = SOM1;
  txbuf[1] = SOM2;
  txbuf[2] = 0x05;
  txbuf[3] = CMD_GET_VERSION;
  txbuf[4] = EOM;
  TsunamiSerial.write(txbuf, 5);

  // Request system info
  txbuf[0] = SOM1;
  txbuf[1] = SOM2;
  txbuf[2] = 0x05;
  txbuf[3] = CMD_GET_SYS_INFO;
  txbuf[4] = EOM;
  TsunamiSerial.write(txbuf, 5);
}

// **************************************************************
void Tsunami::flush(void) {

  int i;

  rxCount = 0;
  rxLen = 0;
  rxMsgReady = false;
  for (i = 0; i < MAX_NUM_VOICES; i++) {
    voiceTable[i] = 0xffff;
  }
  while (TsunamiSerial.available())
    i = TsunamiSerial.read();
  rxTotal++;
}


// **************************************************************
void Tsunami::update(void) {

  int i;
  uint8_t dat;
  uint8_t voice;
  uint16_t track;

  rxMsgReady = false;

  while (TsunamiSerial.available() > 0) {
    dat = TsunamiSerial.read();
    rxTotal++;
    //    Serial.print("Got ");
    //    Serial.print(dat, HEX);
    if ((rxCount == 0) && (dat == SOM1)) {
      rxCount++;
    }
    else if (rxCount == 1) {
      if (dat == SOM2)
        rxCount++;
      else {
        rxCount = 0;
        Serial.print("Bad msg 1\n");
      }
    }
    else if (rxCount == 2) {
      if (dat <= MAX_MESSAGE_LEN) {
        rxCount++;
        rxLen = dat - 1;
      }
      else {
        rxCount = 0;
        Serial.print("Bad msg 2\n");
      }
    }
    else if ((rxCount > 2) && (rxCount < rxLen)) {
      rxMessage[rxCount - 3] = dat;
      rxCount++;
    }
    else if (rxCount == rxLen) {
      if (dat == EOM)
        rxMsgReady = true;
      else {
        rxCount = 0;
        Serial.print("Bad msg 3\n");
      }
    }
    else {
      rxCount = 0;
      Serial.print("Bad msg 4\n");
    }

    if (rxMsgReady) {
      switch (rxMessage[0]) {

        case RSP_TRACK_REPORT:
          track = rxMessage[2];
          track = (track << 8) + rxMessage[1] + 1;
          voice = rxMessage[3];
          if (voice < MAX_NUM_VOICES) {
            if (rxMessage[4] == 0) {
              if (track == voiceTable[voice])
                voiceTable[voice] = 0xffff;
            }
            else {
              started[voice] = millis();
              voiceTable[voice] = track;
            }
          }
          // ==========================
          Serial.print("Track ");
          Serial.print(track);
          Serial.print(", voice ");
          Serial.print(voice);
          if (rxMessage[4] == 0) {
            Serial.print(" off, ");
            Serial.print(millis() -  started[voice]);
            Serial.println(" ms");
          } else
            Serial.print(" on\n");
          for (i = 0; i < MAX_NUM_VOICES; i++)
            if (i != voice && voiceTable[i] != 0xffff) {
              Serial.print(" track ");
              Serial.print(voiceTable[i] );
              Serial.print(" still playing on voice ");
              Serial.println( i );
            }
          // ==========================
          break;

        case RSP_VERSION_STRING:
          for (i = 0; i < (VERSION_STRING_LEN - 1); i++)
            version[i] = rxMessage[i + 1];
          version[VERSION_STRING_LEN - 1] = 0;
          versionRcvd = true;
          // ==========================
          Serial.write(version);
          Serial.write("\n");
          // ==========================
          break;

        case RSP_SYSTEM_INFO:
          numVoices = rxMessage[1];
          numTracks = rxMessage[3];
          numTracks = (numTracks << 8) + rxMessage[2];
          sysinfoRcvd = true;
          // ==========================
          Serial.print("Sys info received\n");
          Serial.print("num voices: ");
          Serial.println(numVoices);
          Serial.print("num tracks: ");
          Serial.println(numTracks);
          // ==========================
          break;

      }
      rxCount = 0;
      rxLen = 0;
      rxMsgReady = false;

    } // if (rxMsgReady)

  } // while (TsunamiSerial.available() > 0)
}

// **************************************************************
bool Tsunami::isTrackPlaying(int trk) {

  int i;
  bool fResult = false;

  update();
  for (i = 0; i < MAX_NUM_VOICES; i++) {
    if (voiceTable[i] == trk)
      fResult = true;
  }
  return fResult;
}

// **************************************************************
void Tsunami::masterGain(int out, int gain) {

  uint8_t txbuf[8];
  unsigned short vol;
  uint8_t o;

  o = out & 0x03;
  txbuf[0] = SOM1;
  txbuf[1] = SOM2;
  txbuf[2] = 0x08;
  txbuf[3] = CMD_MASTER_VOLUME;
  txbuf[4] = o;
  vol = (unsigned short)gain;
  txbuf[5] = (uint8_t)vol;
  txbuf[6] = (uint8_t)(vol >> 8);
  txbuf[7] = EOM;
  TsunamiSerial.write(txbuf, 8);
}

// **************************************************************
void Tsunami::setReporting(bool enable) {

  uint8_t txbuf[6];

  txbuf[0] = SOM1;
  txbuf[1] = SOM2;
  txbuf[2] = 0x06;
  txbuf[3] = CMD_SET_REPORTING;
  txbuf[4] = enable;
  txbuf[5] = EOM;
  TsunamiSerial.write(txbuf, 6);
}

// **************************************************************
bool Tsunami::getVersion(char *pDst, int len) {

  int i;

  update();
  if (!versionRcvd) {
    return false;
  }
  for (i = 0; i < (VERSION_STRING_LEN - 1); i++) {
    if (i >= (len - 1))
      break;
    pDst[i] = version[i];
  }
  pDst[++i] = 0;
  return true;
}

// **************************************************************
int Tsunami::getNumTracks(void) {

  update();
  return numTracks;
}


// **************************************************************
void Tsunami::trackPlaySolo(int trk, int out, bool lock) {

  int flags = 0;

  if (lock)
    flags |= 0x01;
  trackControl(trk, TRK_PLAY_SOLO, out, flags);
}

// **************************************************************
void Tsunami::trackPlayPoly(int trk, int out, bool lock) {

  int flags = 0;

  if (lock)
    flags |= 0x01;
  trackControl(trk, TRK_PLAY_POLY, out, flags);
}

// **************************************************************
void Tsunami::trackLoad(int trk, int out, bool lock) {

  int flags = 0;

  if (lock)
    flags |= 0x01;
  trackControl(trk, TRK_LOAD, out, flags);
}

// **************************************************************
void Tsunami::trackStop(int trk) {

  trackControl(trk, TRK_STOP, 0, 0);
}

// **************************************************************
void Tsunami::trackPause(int trk) {

  trackControl(trk, TRK_PAUSE, 0, 0);
}

// **************************************************************
void Tsunami::trackResume(int trk) {

  trackControl(trk, TRK_RESUME, 0, 0);
}

// **************************************************************
void Tsunami::trackLoop(int trk, bool enable) {

  if (enable)
    trackControl(trk, TRK_LOOP_ON, 0, 0);
  else
    trackControl(trk, TRK_LOOP_OFF, 0, 0);
}

// **************************************************************
void Tsunami::trackControl(int trk, int code, int out, int flags) {

  uint8_t txbuf[10];
  uint8_t o;

  o = out & 0x07;
  txbuf[0] = SOM1;
  txbuf[1] = SOM2;
  txbuf[2] = 0x0a;
  txbuf[3] = CMD_TRACK_CONTROL;
  txbuf[4] = (uint8_t)code;
  txbuf[5] = (uint8_t)trk;
  txbuf[6] = (uint8_t)(trk >> 8);
  txbuf[7] = (uint8_t)o;
  txbuf[8] = (uint8_t)flags;
  txbuf[9] = EOM;
  TsunamiSerial.write(txbuf, 10);
}

// **************************************************************
void Tsunami::stopAllTracks(void) {

  uint8_t txbuf[5];

  txbuf[0] = SOM1;
  txbuf[1] = SOM2;
  txbuf[2] = 0x05;
  txbuf[3] = CMD_STOP_ALL;
  txbuf[4] = EOM;
  TsunamiSerial.write(txbuf, 5);
}

// **************************************************************
void Tsunami::resumeAllInSync(void) {

  uint8_t txbuf[5];

  txbuf[0] = SOM1;
  txbuf[1] = SOM2;
  txbuf[2] = 0x05;
  txbuf[3] = CMD_RESUME_ALL_SYNC;
  txbuf[4] = EOM;
  TsunamiSerial.write(txbuf, 5);
}

// **************************************************************
void Tsunami::trackGain(int trk, int gain) {

  uint8_t txbuf[9];
  unsigned short vol;

  txbuf[0] = SOM1;
  txbuf[1] = SOM2;
  txbuf[2] = 0x09;
  txbuf[3] = CMD_TRACK_VOLUME;
  txbuf[4] = (uint8_t)trk;
  txbuf[5] = (uint8_t)(trk >> 8);
  vol = (unsigned short)gain;
  txbuf[6] = (uint8_t)vol;
  txbuf[7] = (uint8_t)(vol >> 8);
  txbuf[8] = EOM;
  TsunamiSerial.write(txbuf, 9);
}

// **************************************************************
void Tsunami::trackFade(int trk, int gain, int time, bool stopFlag) {

  uint8_t txbuf[12];
  unsigned short vol;

  txbuf[0] = SOM1;
  txbuf[1] = SOM2;
  txbuf[2] = 0x0c;
  txbuf[3] = CMD_TRACK_FADE;
  txbuf[4] = (uint8_t)trk;
  txbuf[5] = (uint8_t)(trk >> 8);
  vol = (unsigned short)gain;
  txbuf[6] = (uint8_t)vol;
  txbuf[7] = (uint8_t)(vol >> 8);
  txbuf[8] = (uint8_t)time;
  txbuf[9] = (uint8_t)(time >> 8);
  txbuf[10] = stopFlag;
  txbuf[11] = EOM;
  TsunamiSerial.write(txbuf, 12);
}

// **************************************************************
void Tsunami::samplerateOffset(int out, int offset) {

  uint8_t txbuf[8];
  unsigned short off;
  uint8_t o;

  o = out & 0x03;
  txbuf[0] = SOM1;
  txbuf[1] = SOM2;
  txbuf[2] = 0x08;
  txbuf[3] = CMD_SAMPLERATE_OFFSET;
  txbuf[4] = (uint8_t)o;
  off = (unsigned short)offset;
  txbuf[5] = (uint8_t)off;
  txbuf[6] = (uint8_t)(off >> 8);
  txbuf[7] = EOM;
  TsunamiSerial.write(txbuf, 8);
}

// **************************************************************
void Tsunami::setTriggerBank(int bank) {

  uint8_t txbuf[6];

  txbuf[0] = SOM1;
  txbuf[1] = SOM2;
  txbuf[2] = 0x06;
  txbuf[3] = CMD_SET_TRIGGER_BANK;
  txbuf[4] = (uint8_t)bank;
  txbuf[5] = EOM;
  TsunamiSerial.write(txbuf, 6);
}

// **************************************************************
void Tsunami::setInputMix(int mix) {

  uint8_t txbuf[6];

  txbuf[0] = SOM1;
  txbuf[1] = SOM2;
  txbuf[2] = 0x06;
  txbuf[3] = CMD_SET_INPUT_MIX;
  txbuf[4] = (uint8_t)mix;
  txbuf[5] = EOM;
  TsunamiSerial.write(txbuf, 6);
}

// **************************************************************
void Tsunami::setMidiBank(int bank) {

  uint8_t txbuf[6];

  txbuf[0] = SOM1;
  txbuf[1] = SOM2;
  txbuf[2] = 0x06;
  txbuf[3] = CMD_SET_MIDI_BANK;
  txbuf[4] = (uint8_t)bank;
  txbuf[5] = EOM;
  TsunamiSerial.write(txbuf, 6);
}
