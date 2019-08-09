
#include <OctoWS2811.h>

const int ledsPerStrip = 174+4*60;

DMAMEM int displayMemory[ledsPerStrip * 6];
int drawingMemory[ledsPerStrip * 6];

const int config = WS2811_GRB | WS2811_800kHz;

OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config);

int rainbowColors[180];


void setup() {
  pinMode(1, OUTPUT);
  digitalWrite(1, HIGH);
 Serial.begin(115200);
  while (!Serial) delay(10);
  for (int i = 0; i < 180; i++) {
    int hue = i * 2;
    int saturation = 100;
    int lightness = 50;
    // pre-compute the 180 rainbow colors
    rainbowColors[i] = makeColor(hue, saturation, lightness);
    
  }
  digitalWrite(1, LOW);
  leds.begin();
  leds.show();
}


void loop() {
  rainbow(10, 2500);
}


// phaseShift is the shift between each row.  phaseShift=0
// causes all rows to show the same colors moving together.
// phaseShift=180 causes each row to be the opposite colors
// as the previous.
//
// cycleTime is the number of milliseconds to shift through
// the entire 360 degrees of the color wheel:
// Red -> Orange -> Yellow -> Green -> Blue -> Violet -> Red
//
void rainbow(int phaseShift, int cycleTime)
{
  int color, x, y, offset, wait;
  wait = cycleTime * 1000 / ledsPerStrip;
  for (color = 0; color < 180; color++) {
   int maxLED = (millis() / 300) % 2 == 0 ? 174 : 173;
   digitalWrite(1, HIGH);
    for (y = 0; y < 8; y++) {
      for (x = 0; x < ledsPerStrip; x++) {

        int index = (color + x + y * phaseShift / 2) % 180;
        leds.setPixel(x + y * ledsPerStrip, rainbowColors[index]);
      }
      if ((millis() / 300) % 2 == 0 )
         leds.setPixel(173 + y * ledsPerStrip, 0);
    }
    leds.show();
    digitalWrite(1, LOW);
    delayMicroseconds(wait);
  }
}
