
#include "config.h"

TTGOClass *ttgo;

char buf[128];
bool rtcIrq = false;

void setup()
{
    ttgo = TTGOClass::getWatch();
    ttgo->begin();
    ttgo->openBL();
    ttgo->rtc->setDateTime(2022, 3, 23, 18, 34, 53);
    Serial.begin(115200);
}

void loop(){
    Serial.println(ttgo->rtc->formatDateTime());
    ttgo->tft->setTextColor(TFT_YELLOW, TFT_BLACK);
    snprintf(buf, sizeof(buf), "%s", ttgo->rtc->formatDateTime());
    ttgo->tft->drawString(buf, 5, 118, 7);
    delay(1000);
}
