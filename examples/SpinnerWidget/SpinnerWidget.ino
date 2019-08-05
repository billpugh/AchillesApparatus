
/*
   At each time step:
   get current position.
   plot predicted and current position
   Calculate effective angular velocity
   Plot desired and actual velocity
   calculate angular acceleration from gravity and pusher

*/

#include <math.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "log.h"

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

const int FEEDBACK_PIN = 12;

const int SERVOMIN = 265; // this is the 'minimum' pulse length count (out of 4096)
const int  SERVOMAX  = 350; // this is the 'maximum' pulse length count (out of 4096)
const int  SERVOMIN_TOP = 296;
const int SERVOMAX_BOTTOM = 308;
const int SERVO_STILL = 302;
const int MAX_FORWARD_RPM = 180;
const int MAX_BACKWARD_RPM = 128;

const int Hz = 50;

double targetRPM = 0;



// get position, measured as 0 - 1000

int getPosition() {
  while (1) {
    while (digitalRead(FEEDBACK_PIN));
    // control signal is now low
    while (!digitalRead(FEEDBACK_PIN));
    unsigned long startHigh = micros();
    while (digitalRead(FEEDBACK_PIN));
    unsigned long endHigh = micros();
    while (!digitalRead(FEEDBACK_PIN));
    unsigned long endCycle = micros();
    unsigned long cycleLength = endCycle - startHigh;
    if (cycleLength > 1000 && cycleLength < 1200) {
      int v = (endHigh - startHigh) * 1000 / 1099;
      int v2 = map(v, 28, 963, 0, 1000);
      if (false) {
        Serial.println(v2);
        Serial.print(millis());
        Serial.print(" ");

      }
      return constrain(v2, 0, 1000);
    }
  }

}


int posDiff(int p1, int p2) {
  int diff = abs(p1 - p2);
  if (diff < 800) return diff;
  return abs(diff - 1000);
}

void moveToPosition(int target) {
  int p = getPosition();
  if (posDiff(p, target) < 10) return;
  setRPM(0,  20);

  while (true) {
    delay(50);
    int nextP = getPosition();
    if (nextP == p) continue;
    p = nextP;
    logf("at %3d, moving towards %3d\n", p, target);

    if (posDiff(p, target) < 10) break;

  }
  setRPM(0,  0);

}


void setup() {
  pwm.begin();

  pwm.setPWMFreq(Hz);  // Analog servos run at ~60 Hz updates
  pwm.setPWM(0, 0, SERVO_STILL);
  Serial.begin(115200);
  while (!Serial) delay(10);
  Serial.println("8 channel Servo test!");


  delay(10);

  pinMode(FEEDBACK_PIN, INPUT);
  moveToPosition(250);
  setRPM(0, (int) targetRPM);
}

/*
   Set the RPM for a servo
*/
void setRPM(int servo, int RPM) {
  int v;
  //Serial.println(RPM);
  RPM = constrain(RPM, -MAX_BACKWARD_RPM, MAX_FORWARD_RPM);
  if (RPM == 0)
    v = SERVO_STILL;
  else if (RPM < 0)
    v = map(RPM, -MAX_BACKWARD_RPM, -1, SERVOMIN, SERVOMIN_TOP + 1);
  else v = map(RPM, 1, MAX_FORWARD_RPM, SERVOMAX_BOTTOM - 1, SERVOMAX);
  pwm.setPWM(servo, 0, v);
}


const int BUFFER_SIZE = 3;
int16_t posBuffer[BUFFER_SIZE];
long timeBuffer[BUFFER_SIZE];
unsigned long nextPositionMeasurement = 0;

int bufferPosition = 0;


bool haveRPM = false;
int16_t lastRPM = 0;
bool havePos = false;
int16_t lastPos = 0;

void gotPosition(int16_t pos) {
  if (false)
    logf("got pos: %d\n", pos);
  havePos = true;
  lastPos = pos;

}

void gotRPM(int16_t pos, int16_t rpm) {
  haveRPM = true;
  lastRPM = rpm;
  //logf("got RPM: %4d %4d\n", pos, rpm);
}

void measurePosition() {
  unsigned long nowMS = millis();
  if (nowMS < nextPositionMeasurement) return;
  nextPositionMeasurement = nowMS + 50;

  int16_t pos = getPosition();
  gotPosition(pos);

  int i3 =  (bufferPosition) % BUFFER_SIZE;
  posBuffer[i3] = pos;
  timeBuffer[i3] = micros();

  if (bufferPosition < 3) {
    logf("just starting: %d\n", bufferPosition);
    bufferPosition++;
    return;
  }
  int i1 = (bufferPosition - 2) % BUFFER_SIZE;
  int i2 =  (bufferPosition - 1) % BUFFER_SIZE;

  bufferPosition++;

  int16_t pos1 = posBuffer[i1];
  int16_t pos2 = posBuffer[i2];
  int16_t pos3 = posBuffer[i3];

  long time1 = timeBuffer[i1];
  long time2 = timeBuffer[i2];
  long time3 = timeBuffer[i3];

  if (false)
    logf("checking: %4d %4d %4d  %d %d %d\n", pos1, pos2, pos3, time1, time2, time3);


  if (time1 >= time2 || time2 >= time3) {
    //Serial.println("time glitch ");
    return;
  } else if (pos1 <= pos2 && pos2 <= pos3) {
    //Serial.println("easy forward ");

  } else  if (pos3 <= pos2 && pos2 <= pos1) {
    //Serial.println("easy backwards ");

  } else if (pos1 < pos2 && pos2 > 600 && pos3 < 400) {
    pos3 += 1000;
  } else if (pos2 < pos3 && pos1 > 600 && pos2 < 400) {
    pos1 -= 1000;
  } else if (pos3 < pos2 && pos2 > 600 && pos1 < 400) {
    pos1 += 1000;
  } else if (pos2 < pos1 && pos3 > 600 && pos2 < 400) {
    pos3 -= 1000;
  } else  if (abs(pos3 - pos1) <= 2
              || abs(pos3 - pos2) <= 2 && abs(pos2 - pos1) > 600
              ||  abs(pos1 - pos2) <= 2 && abs(pos2 - pos3) > 600) {
    gotRPM(pos3, 0);
    return;
  } else {
    logf("RPM calculation fail: %d %d %d  %d %d %d\n", pos1, pos2, pos3, time1, time2, time3);
    return;
  }
  if (false)
    logf("calculate: %4d %4d %4d  %d %d\n", pos1, pos2, pos3, time2 - time1, time3 - time2);

  int32_t speed13 = (pos3 - pos1) * 1000000L / (time3 - time1);

  if (false) {
    int32_t speed12 = (pos2 - pos1) * 1000000L / (time2 - time1);
    int32_t speed23 = (pos3 - pos2) * 1000000L / (time3 - time2);
    logf("calc:: %d  %d  %d %d %d\n", millis(), pos3, speed12, speed23, speed13);
  }
  gotRPM(pos3,  -60 * speed13 / 1000 );


}
unsigned long lastUpdate = 0;

const double twoPi = 6.2831853072;

void loop() {
  measurePosition();
  unsigned long now = millis();
  if (!haveRPM) {
    delay(1);
    return;
  }
  unsigned long deltaT = lastUpdate == 0 ? 0  : now - lastUpdate;
  haveRPM = false;
  lastUpdate = now;
  logf("RPM: %4d %4d %3d %d\n", deltaT, lastPos, lastRPM, (int)targetRPM );
  double angle =  lastPos * twoPi / 1000;
  double accel = sin(angle) * 49;
  double deltaRPM = accel / twoPi * 60 * deltaT / 1000;
  targetRPM += deltaRPM;
  targetRPM = constrain(targetRPM, -100, 100);
  logf("RPM update: %3d %d %3d\n", (int)( angle * 360 / twoPi), (int)accel, (int) targetRPM );
  setRPM(0,  targetRPM);
  delay(1);

}
