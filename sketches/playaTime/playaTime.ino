// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h"

RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void setup () {



  Serial.begin(9600);
  while (!Serial) delay(1);

  
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (false &&  rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    DateTime compiled(F(__DATE__), F(__TIME__));
    TimeSpan adjustment(0,-3,0,7);
    DateTime playaTime = compiled + adjustment;
    
    Serial.println(rtc.adjust(playaTime));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // Serial.println(rtc.adjust(DateTime(2019, 8, 8, 23, 33, 0)));
  }
}

enum Sunlight { NIGHT, MORNING_TWILIGHT, DAWN, DAY, DUSK, EVENING_TWILIGHT, UNKNOWN };
// light levels: Night: -4
// Twilight: -2
// Dusk/Dawn: -1
// Daytime: 0

int div30(int x) {
   return (x+3000)/30 - 100;
}
void loop () {
    DateTime now = rtc.now();
    int hour = now.hour();
    int totalMinutes = now.hour() *60 + now.minute();
    const int sunRise = 6*60+20;
    const int sunSet =19 * 60 +  38;
    int period;
    int lightLevel;
    if (hour < 12) {
      // AM
      Serial.println(totalMinutes - sunRise);
      period = constrain(div30(totalMinutes - sunRise), -2, 1)+2;
      lightLevel = constrain(div30(totalMinutes - sunRise), -3, 1)-1;
    } else {
      // PM
      Serial.println(totalMinutes - sunRise);
      period = constrain(div30(totalMinutes - sunSet), -2, 1)+5;
      if (period == 6) period = 0;
       lightLevel = constrain(div30(sunSet - totalMinutes), -2, 2)-2;
      
      
    }
    Sunlight sunlight = (Sunlight) period;
    Serial.print("sunlight: ");
    Serial.println(sunlight);
    Serial.print("light level: ");
    Serial.println(lightLevel);
    
    
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    
   
    Serial.println();
    delay(3000);
}
