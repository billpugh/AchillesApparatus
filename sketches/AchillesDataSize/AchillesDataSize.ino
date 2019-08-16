#include "Achilles.h"
#include "AchillesLog.h"

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);
  aalog("Achilles data size\n");
  aalogf("FromWidgetData size: %d\n", sizeof(FromWidgetData));
  aalogf("toWidgetData size: %d\n", sizeof(ToWidgetData));
}

void loop() {
  // put your main code here, to run repeatedly:

}
