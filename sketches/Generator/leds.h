
#include <Adafruit_NeoPixel.h>

void setupLEDs();
void setDigitalMeter(int voltage);
void updateDisplays();
bool isRebooting();
void showErr();


extern Adafruit_NeoPixel buttons;

uint16_t getMeterColor();
