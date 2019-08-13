#include <Servo.h>
#include "Achilles.h"
#include "AchillesLog.h"
#include "AchillesWedge.h"

#define NUM_SERVOS          6
#define NUM_BUTTONS         6

#define READ_BUTTONS_MODE   0
#define MOVE_SERVOS_MODE    1

#define CW_SLOW             86
#define CW_FAST             62
#define CCW_SLOW            97
#define CCW_FAST           121
#define CLOSE_ATTEMPTS      20
#define RESET_TIME         120
#define SCRAMBLES            5

#define RESET_SOUND          1
#define GEAR_SOUND           2
#define WIN_1_SOUND          3
#define WIN_2_SOUND          4
#define WIN_3_SOUND          5

Servo servo[NUM_SERVOS];

byte servoControl [] = {3, 4, 5, 6, 9, 10};
byte servoFeedback [] = {14, 15, 16, 17, 20, 21};
int servoGoal [] = {0, 0, 0, 0, 0, 0};
byte servoClose [] = {0, 0, 0, 0, 0};
bool servoActive [] = {false, false, false, false, false, false};
byte buttons[] = {2, 7, 8, 12, 11, 22};

bool buttonState[] = {false, false, false, false, false, false};
byte wheelState[] = {0, 0, 0, 0, 0, 0};
byte difficulty = 2;
unsigned long lastInteraction;
bool resetFlag = false;

unsigned long lastMessageReported = 0;

// measure servo position
int readServoAngle(int pwmPin) {
  int tHigh;
  int tLow;
  int tCycle;
  
  float theta = 0;
  float dc = 0;
  int unitsFC = 360;
  float dcMin = 0.029;
  float dcMax = 0.971;
  float dutyScale = 1;
 
  while(1) {
    pulseIn(pwmPin, LOW);
    tHigh = pulseIn(pwmPin, HIGH);
    tLow =  pulseIn(pwmPin, LOW);
    tCycle = tHigh + tLow;
    if ((tCycle > 1000) && ( tCycle < 1200)) {
      break;
    }
 }

 dc = (dutyScale * tHigh) / tCycle;
 theta = ((dc - dcMin) * unitsFC) / (dcMax - dcMin);
 return theta;
}

void turnServo(byte i, int goal) {
  int pos = readServoAngle(servoFeedback[i]);
  int dist = ((goal - pos + 540) % 360) - 180;
  int ss;

  servoGoal[i] = goal;

  if (dist != 0) {
    servoActive[i] = true;

    if (dist > 1) {
      ss = map(abs(dist), 0, 180, CW_SLOW, CW_FAST);
      servo[i].write(90 - ss);
    } else {
      ss = map(abs(dist), 0, 180, CCW_SLOW, CCW_FAST);
      servo[i].write(90 + ss); 
    }
  }
}

void resetPuzzle(bool spin) {
  bool complete = false;
  byte index;
  byte left;
  byte right;

  if (spin) {
    // pause for a second
    servo[0].write(90);
    servo[1].write(90);
    servo[2].write(90);
    servo[3].write(90);
    servo[4].write(90);
    servo[5].write(90);
  
    delay(1000);
  
    // spin like crazy!
    servo[0].write(20);
    servo[1].write(160);
    servo[2].write(20);
    servo[3].write(160);
    servo[4].write(20);
    servo[5].write(160);
  
    delay(3000);
  }
  
  while (complete == false) {
    wheelState[0] = 0;
    wheelState[1] = 0;
    wheelState[2] = 0;
    wheelState[3] = 0;
    wheelState[4] = 0;
    wheelState[5] = 0;
    Serial.print("Scrambling puzzle! Difficulty: ");
    Serial.println(difficulty);

    for (byte i=0; i<SCRAMBLES; ++i) {
      index = random(6);
      Serial.print(index);
      left = (index - 1 + 6) % 6;
      right = (index + 1) % 6;
      wheelState[left] = (wheelState[left] - 1 + difficulty) % difficulty;
      wheelState[right] = (wheelState[right] - 1 + difficulty) % difficulty;
    }
    Serial.println("");

    if (wheelState[0] != wheelState[1] || \
        wheelState[0] != wheelState[2] || \
        wheelState[0] != wheelState[3] || \
        wheelState[0] != wheelState[4] || \
        wheelState[0] != wheelState[5]) {

        complete = true;
    }
  }

  turnServo(0, wheelState[0] * 60);
  turnServo(1, wheelState[1] * 60);
  turnServo(2, wheelState[2] * 60);
  turnServo(3, wheelState[3] * 60);
  turnServo(4, wheelState[4] * 60);
  turnServo(5, wheelState[5] * 60);
}

void triggerWheels(byte index) {
  byte left;
  byte right;
  
  left = (index - 1 + 6) % 6;
  right = (index + 1) % 6;
  wheelState[left] = (wheelState[left] + 1) % difficulty;
  wheelState[right] = (wheelState[right] + 1) % difficulty;

  playSound(GEAR_SOUND, false);

  turnServo(left, wheelState[left] * 60);
  turnServo(right, wheelState[right] * 60);

}

bool checkPuzzleSolved() {
  return (wheelState[0] == 0 && wheelState[1] == 0 && \
          wheelState[2] == 0 && wheelState[3] == 0 && \
          wheelState[4] == 0 && wheelState[4] == 0);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(5000);
  Serial.println("Hexaplexor Achilles Appartus widget");
  setupComm(0x72);

  for (byte i=0; i<NUM_SERVOS; ++i) {
    servo[i].attach(servoControl[i]);
    pinMode(servoFeedback[i], INPUT);
  }

  pinMode(buttons[0], INPUT_PULLUP);
  pinMode(buttons[1], INPUT_PULLUP);
  pinMode(buttons[2], INPUT_PULLUP);
  pinMode(buttons[3], INPUT_PULLUP);
  pinMode(buttons[4], INPUT_PULLUP);
  pinMode(buttons[5], INPUT_PULLUP);

  turnServo(0, 0);
  turnServo(1, 0);
  turnServo(2, 0);
  turnServo(3, 0);
  turnServo(4, 0);
  turnServo(5, 0);

  randomSeed(analogRead(23));
  lastInteraction = millis();
  resetFlag = true;
}

unsigned long lastCommComplaint = 0;
unsigned long lastStatusReport = 0;

void loop() {
  byte i;
  byte mode = READ_BUTTONS_MODE;
  int goal;
  int ss;
  int dist;
  int pos;
  
  unsigned long now = millis();

  if (!commOK() && now > 2000 &&  now - 2000 > lastCommComplaint) {
    lastCommComplaint = now;
    logf("Comm not working at %d\n", now);
  }
  if (commOK() && now > 5000 &&  now - 5000 > lastStatusReport) {
    lastStatusReport = now;
    logf("System mode: %s\n", systemModeName(getSystemMode()));
    logf("Daytime : %s\n", daytimeName(getDaytime()));
    logf("Light level : %d\n", getLightLevel());
  }

  for (i=0; i<NUM_SERVOS; ++i) {
    if (servoActive[i] == true) {
      mode = MOVE_SERVOS_MODE;
      break;
    }
  }
  
  if (mode == MOVE_SERVOS_MODE) {
    for (i=0; i<NUM_SERVOS; ++i) {
      if (servoActive[i] == true) {
        goal = servoGoal[i];
        pos = readServoAngle(servoFeedback[i]);
        dist = ((goal - pos + 540) % 360) - 180;
    
        if (dist > 1) {
          ss = map(abs(dist), 0, 180, CW_SLOW, CW_FAST);
          servo[i].write(ss);
        } else {
          ss = map(abs(dist), 0, 180, CCW_SLOW, CCW_FAST);
          servo[i].write(ss); 
        }
  
        if (abs(dist) < 5) {
          servo[i].write(90);
          servoClose[i] += 1;
          if (servoClose[i] == CLOSE_ATTEMPTS) {
            servoActive[i] = false;
            servoClose[i] = 0;
          }
        }
      }
    }
  } else {
    
    
    if (millis() - lastInteraction > RESET_TIME * 1000) {
      resetFlag = true;
    }
    
    if (resetFlag) {
      difficulty = 2;
      clearAllPoints();
      resetPuzzle(false);
      lastInteraction = millis();
      resetFlag = false;
      playSound(RESET_SOUND, false);
      
    } else {

      if (checkPuzzleSolved()) {
        if (difficulty == 2) {
          difficulty = 3;
          setPointTo(1);
          playSound(WIN_1_SOUND, true);
          resetPuzzle(true);
          
        } else if (difficulty == 3) {
          difficulty = 6;
          setPointTo(15);
          playSound(WIN_2_SOUND, true);
          resetPuzzle(true);
          
        } else if (difficulty == 6) {
          difficulty = 2;
          setPointTo(255);
          playSound(WIN_3_SOUND, true);
          resetPuzzle(true);
        }
      } else {
        // read buttons to trigger wheel turning
        for (i=0; i<NUM_BUTTONS; ++i) {
  
          if (digitalRead(buttons[i]) == LOW) {
            if (buttonState[i] == false) {
              triggerWheels(i);
              lastInteraction = millis();
            }
            buttonState[i] = true;
          } else {
            buttonState[i] = false;
          }  
        }
      }
    }
  }
}

