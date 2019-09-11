#include <Arduino.h>
#include<FastLED.h>
#include "animations.h"
#include "printf.h"

#include <SdFat.h>
#include <Time.h>

SdFatSdioEX sd;

FatFile dirFile;
const uint8_t NUM_STRIPS  = 8;

float towerTime() {
  return millis() / 1000.0;
}

const uint8_t BRIGHTNESS_NORMAL = 80;
const uint8_t BRIGHTNESS_BORED = 40;
int opcFrame;
CRGB unusedLED;
CRGB leds[NUM_STRIPS * ledsPerStrip];

int reflectedLED(int p, int ledHeight) {
  int fromTop = ledHeight - p; // expect positive value
  if (fromTop > reflectedLEDs) return -1;
  int reflected =  ledHeight - 1 + fromTop;
  if (reflected < 0 || reflected >= ledsPerStrip) return -1;
  return reflected;
}



void copyLEDs() {
  uint16_t halfWay = 4 * ledsPerStrip;
  for (int i = 0; i < halfWay; i++) {
    leds[i + halfWay] =  leds[i];
  }
}


void setTowerPixel(int p, const CRGB& color) {
  if (p >= tallTowerHeight) return;
  leds[p] = color;
  int reflected = reflectedLED(p, tallTowerHeight);
  if (reflected >= 0) leds[reflected] = color;

  int shortP =  p;
  if (shortP > shortTowerHeight)
    return;

  leds[shortP + ledsPerStrip] = color;
  leds[shortP + 2 * ledsPerStrip] = color;
  reflected = reflectedLED(shortP, shortTowerHeight);
  if (reflected >= 0) {
    leds[reflected + ledsPerStrip] = color;
    leds[reflected + 2 * ledsPerStrip] = color;
  }


}


const size_t MAX_FILE_NAME_LENGTH = 30;

boolean opcOK = true;
class AnimationFile {
  public:
    AnimationFile() {
      name[0] = '\0';
    };
    char name[MAX_FILE_NAME_LENGTH];
};
int numberOfAnimations = 0;
int numberOfOPCFiles = 0;


AnimationFile * animationFile;

boolean isOPC(FatFile f) {
  char name[MAX_FILE_NAME_LENGTH];
  f.getName(name, MAX_FILE_NAME_LENGTH);

  const char *suffix = strrchr(name, '.');

  if ( strcmp(suffix, ".opc") == 0)
    return true;
  return false;
}


int countAnimations() {
  return numberOfOPCFiles;
}
void setLEDsToBlack() {
  for (int j = 0; j < NUM_STRIPS * ledsPerStrip; j++)
    leds[j] = CRGB::Black;
}
const enum EOrder LED_COLOR_ORDER = GRB;

volatile boolean animationsSetUp = false;
void setupAnimations() {
  for (int i = 0; i < tallTowerHeight; i++) {
    int r =  reflectedLED(i,  tallTowerHeight);
    if (r >= 0)
      myprintf("Reflected %d -> %d\n", i, r);
  }
  LEDS.addLeds<WS2811_PORTD, NUM_STRIPS, LED_COLOR_ORDER>(leds, ledsPerStrip);
  LEDS.setBrightness(240);
  LEDS.clear();
  setLEDsToBlack();
  LEDS.show();

  LEDS.setCorrection(TypicalLEDStrip);
  Serial.println("setupAnimations");
  sd.begin();

  if (!dirFile.open(sd.vwd(), "opcFiles", O_READ)) {
    opcOK = false;
    Serial.println("Open opcFiles / failed, ignoring animations");
    return;
  }
  FatFile file;
  while (file.openNext(&dirFile, O_READ)) {
    if (isOPC(file))
      numberOfOPCFiles++;
    file.close();
  }
  dirFile.rewind();
  myprintf("Got %d animations\n", numberOfOPCFiles);
  if (numberOfOPCFiles == 0) return;

  animationFile = new AnimationFile[numberOfOPCFiles];
  int i = 0;
  while (file.openNext(&dirFile, O_READ)) {
    if (isOPC(file)) {

      file.getName(animationFile[i].name, MAX_FILE_NAME_LENGTH);
      myprintf( "found %d %s\n", i, animationFile[i].name);
      i++;
    }
    file.close();
  }
  numberOfAnimations = countAnimations();
  myprintf(" %d opc files, %d total animations\n", numberOfOPCFiles,
           numberOfAnimations);
  nextAnimation();
  animationsSetUp = true;
}

class RotateRainbowUp : public Animation {

    uint8_t hue = 0;
  public:
    RotateRainbowUp()  {
      kind = BASIC_KIND;
    };
    void initialize(int idx) {
      index = idx;
    }
    virtual void printName() {
      myprintf("Rainbow up animation %d\n", index);
    }
    virtual void update(unsigned long now) {
      uint8_t v = 220;
      for (int x = 0; x < tallTowerHeight; x++) {
        CRGB led = CHSV(x * 8 + hue, 255, v);
        setTowerPixel(x, led);
      }
      hue++;
    }
};

RotateRainbowUp animationRotateRainbowUp;


uint8_t header[4];
uint8_t buf[3000];
class ShowOPC : public Animation {

    FatFile file;
    int frameRate;
    int bytesRead = 0;
    boolean ok;

  public:
    ShowOPC()  {
      kind = OPC_KIND;
    };
    virtual void printName() {

      myprintf("Opc animation %d\n", index);
      AnimationFile thisAnimation = animationFile[index];
      myprintf("file %s\n",  thisAnimation.name );
    }
    boolean readFirstHeader() {
      int count = file.read(header, 4);

      if (count != 4) {
        Serial.print(count);
        Serial.println(" bytes read");

        return false;
      }
      if (header[0] != 'O' || header[1] != 'P' || header[2] != 'C') {
        Serial.println("Not OPC file");

        return false;
      }
      frameRate = header[3];
      if (frameRate == 0)
        frameRate = 30;
      bytesRead += 4;
      myprintf("set up opc animation %d\n", index);
      return true;
    }
    void initialize(int idx) {
      index = numberOfOPCFiles == 0 ? 0 : (idx) % numberOfOPCFiles;
      myprintf("Initialize opc. index = %d, opc = %d\n", index, idx);
      ok = false;
      bytesRead = 0;
      AnimationFile thisAnimation = animationFile[index];
      myprintf("Opening %s\n",  thisAnimation.name );
      if (!file.open(&dirFile, thisAnimation.name , O_READ)) {
        myprintf("Could not open %s\n", thisAnimation.name );
        return;
      }


      if (readFirstHeader())
        ok = true;
      opcFrame = 0;
    }
    void close() {
      file.close();
    }
    boolean isOK()  {
      return ok;
    }
    void update(unsigned long now) {

      if (!ok) {
        animationRotateRainbowUp.update(now);
        return;
      }

      int count = file.read(header, 4);
      if (count == 0 && bytesRead > 10000) {
        myprintf("Rewinding after reading %d bytes, %dms to next EPOC\n", bytesRead,
                 millisToNextEpoc());
        AnimationFile thisAnimation = animationFile[index];
        myprintf("Opening %s\n",  thisAnimation.name );

        file.rewind();
        bytesRead = 0;
        opcFrame = 0;
        readFirstHeader();
        count = file.read(header, 4);
      } else
        opcFrame++;
      if (count != 4) {
        Serial.print(count);
        Serial.println(" header bytes read");
        ok = false;
        return ;
      }
      bytesRead += count;
      uint16_t length = (header[2] << 8) |  header[3] ;
      count = file.read(buf, length);
      bytesRead += count;
      if (count != length) {
        myprintf(" %d frame bytes read, %d expected\n", count, length);
        ok = false;
        return ;
      }

      for (int led = 0; led < tallTowerHeight; led++) {
        int pos = led * 6;
        setTowerPixel(led, CRGB(buf[pos], buf[pos + 1], buf[pos + 2]));
      }
      setTowerPixel(tallTowerHeight - 1, CRGB(0, 255, 0));

    }

};

int nextOPC = 0;


ShowOPC animationShowOPC;

Animation * currentAnimation = &animationRotateRainbowUp;


Animation * getAnimation() {

  int index = animationEPOC % numberOfAnimations;
  myprintf("get animation %d %d %d\n", numberOfAnimations, animationEPOC, index);
  Animation * result =  & animationShowOPC;
  result->initialize(index);
  return result;

}


void nextAnimation() {
  Serial.println("\nnext animation");

  if (currentAnimation != NULL)
    currentAnimation->close();

  currentAnimation = getAnimation();


}

const int ANIMATION_EPOC_SECONDS = 300;
int animationEPOC;


long millisToNextEpoc() {
  double timeRemaining = (animationEPOC + 1)  * ANIMATION_EPOC_SECONDS
                         - towerTime();
  if (false) {
    myprintf("current epoc started at: % 8d\n", animationEPOC * ANIMATION_EPOC_SECONDS);
    myprintf("next epoc starts at:     % 8d\n", (animationEPOC + 1) * ANIMATION_EPOC_SECONDS);
    Serial.print("time remaining: ");
    Serial.println (timeRemaining);
  }
  long result = (long)(1000 * timeRemaining);
  return result;
}


unsigned long updateAnimation(unsigned long now) {
  long remaining = millisToNextEpoc();
  if (remaining >= 0)
    return remaining;
  int oldEpoc = animationEPOC;
  while (remaining < 0) {
    animationEPOC++;

    remaining += 1000 * ANIMATION_EPOC_SECONDS;
  }
  myprintf("Advanced % d animations to animation epoc % d\n",
           animationEPOC - oldEpoc,
           animationEPOC);
  nextAnimation();
  return remaining;
};
