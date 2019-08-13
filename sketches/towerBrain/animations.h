#include<FastLED.h>

const int ledsPerStrip = 174 + 4 * 60;

const int reflectedLEDs = 60;

const int tallTowerHeight = ledsPerStrip - reflectedLEDs;
const int shortTowerHeight = 4 * 60 - reflectedLEDs;



enum AnimationKind { OPC_KIND,  BASIC_KIND };

extern int opcFrame;
extern int numberOfAnimations;
extern int numberOfOPCFiles;

class Animation {
  public:
    int index;
    enum AnimationKind kind;
    virtual void initialize(int index) {};
    virtual void update(unsigned long now) {};
    virtual void close() {};
    virtual boolean isOK() {
      return true;
    }
    virtual void printName() {
      Serial.println("abstract animation");
    }
};




extern void nextAnimation();

extern boolean scheduleSetUp;

extern Animation * currentAnimation;
extern void setupAnimations();
extern unsigned long updateAnimation(unsigned long now);
extern int animationEPOC;
extern long millisToNextEpoc() ;
void copyLEDs();
