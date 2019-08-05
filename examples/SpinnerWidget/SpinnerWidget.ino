
/*
   At each time step:
   get current position.
   plot predicted and current position
   Calculate effective angular velocity
   Plot desired and actual velocity
   calculate angular acceleration from gravity and pusher

*/

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

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

void setup() {
  pwm.begin();

  pwm.setPWMFreq(Hz);  // Analog servos run at ~60 Hz updates
  pwm.setPWM(0, 0, SERVO_STILL);
  Serial.begin(115200);
  while (!Serial) delay(10);
  Serial.println("8 channel Servo test!");


  delay(10);
  
  pinMode(FEEDBACK_PIN, INPUT);
}


/*
 * Set the RPM for a servo
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


// get position, measured as 0 - 1000

int getPosition() {
  while (digitalRead(FEEDBACK_PIN));
  // control signal is now low
  while (!digitalRead(12));
  unsigned long startHigh = micros();
  while (digitalRead(12));
  unsigned long endHigh = micros();
  int v = (endHigh - startHigh) * 1000 / 1099;
  int v2 = map(v, 28, 963, 0, 1000);
  if (false) {
    Serial.println(v2);
    Serial.print(millis());
    Serial.print(" ");

  }
  return constrain(v2, 0, 1000);
}


// Calculate RPM
// Right now, it uses a fairly large delay between readings, in order to 
// get reasonable accuracy. We probably want to redesign this up so that it is non-blocking
// and allows us to do other things between measuing the position
int32_t calculateRPM() {
  int attempts = 0;
  boolean found, forward;
  int32_t pos1, pos2, pos3;
  long time1, time2, time3;
  uint16_t timeDelay = 50;
  do {
    found = true;
    pos1 = getPosition();
    time1 = micros();
    delay(timeDelay);
    pos2 = getPosition();
    time2 = micros();
    delay(timeDelay);
    pos3 = getPosition();
    time3 = micros();
    delay(timeDelay);
    if (false) {
      Serial.print(pos1); Serial.print(" "); Serial.print(time1); Serial.print(" ");
      Serial.print(pos2); Serial.print(" "); Serial.print(time2); Serial.print(" ");
      Serial.print(pos3); Serial.print(" "); Serial.print(time3); Serial.print(" ");
      Serial.println();
    }

    bool forward;
    if (time1 >= time2 || time2 >= time3) {
      //Serial.println("time glitch");
      found = false;
    } else if (pos1 < pos2 && pos2 < pos3) {
      //Serial.println("easy forward ");
      forward = true;
    } else  if (pos3 < pos2 && pos2 < pos1) {
      //Serial.println("easy backwards ");
      forward = false;
    } else if (pos1 < pos2 && pos2 > 600 && pos3 < 400) {
      forward = true;
      pos3 += 1000;
    } else if (pos2 < pos3 && pos1 > 600 && pos2 < 400) {
      forward = true;
      pos1 -= 1000;
    } else if (pos3 < pos2 && pos2 > 600 && pos1 < 400) {
      forward = false;
      pos1 += 1000;
    } else if (pos2 < pos1 && pos3 > 600 && pos2 < 400) {
      forward = false;
      pos3 -= 1000;
    } else  if (attempts > 5 || abs(pos3 - pos1) <= 2) {
      return 0;
    } else {
      Serial.print("retry: ");
      Serial.print(pos1);
      Serial.print(" ");
      Serial.print(pos2);
      Serial.print(" ");
      Serial.println(pos3);

      attempts++;
      found = false;
    }

  } while (!found);

  int32_t speed12 = (pos2 - pos1) * 1000000 / (time2 - time1);
  int32_t speed23 = (pos3 - pos2) * 1000000 / (time3 - time2);
  int32_t speed13 = (pos3 - pos1) * 1000000 / (time3 - time1);

  int32_t x = (pos3 - pos2) ;
  int32_t y = (time3 - time2);
  int32_t speed23a = x * 1000000 / y;
  if (false) {
    Serial.print("calc:: ");
    Serial.print(millis());
    Serial.print(" ");
    Serial.print(pos2);
    Serial.print(" ");
    Serial.print(speed12);
    Serial.print(" ");
    Serial.print(speed23);
    Serial.print(" ");
    Serial.print(speed13);
    Serial.println();
  }
  return -60 * speed13 / 1000;
}

void loop() {
  for (int rpm = 1; rpm < 100; rpm++) {
    setRPM(0,  rpm);
    delay(50);
    Serial.print(rpm);
    Serial.print(" ");
    Serial.println(calculateRPM());

  }
  setRPM(0,  0);
  delay(1000);
  for (int rpm = -1; rpm > -100; rpm--) {
    setRPM(0,  rpm);
    delay(50);
    Serial.print(rpm);
    Serial.print(" ");
    Serial.println(calculateRPM());

  }
  setRPM(0,  0);
  delay(1000);

}
