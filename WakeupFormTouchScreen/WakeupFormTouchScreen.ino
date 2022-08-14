#include "config.h"

TTGOClass *ttgo;

char buf[128];
bool irq = false;
bool rtcIrq = false;
bool firstboot = true;

int battery(){
  ttgo->power->adc1Enable(AXP202_VBUS_VOL_ADC1 | AXP202_VBUS_CUR_ADC1 | AXP202_BATT_CUR_ADC1 | AXP202_BATT_VOL_ADC1, true);
  return ttgo->power->getBattPercentage();
  }

void clk(){
    ttgo->tft->setTextColor(TFT_GREEN, TFT_BLACK);
    snprintf(buf, sizeof(buf), "%s", ttgo->rtc->formatDateTime());
    ttgo->tft->drawString(buf, 5, 118, 7);

    char per[16];
    sprintf(per, "%d \%", battery());
    ttgo->tft->drawString(per, 210, 0, 2);
    //ttgo->tft->drawString("%", 210, 0, 3);
    //Serial.println(ttgo->rtc->formatDateTime());
 }

void setup()
{
  
    Serial.begin(115200);
    ttgo = TTGOClass::getWatch();
    ttgo->begin();
    ttgo->openBL();
    Serial.printf("[%d]\n",ttgo->power->readIRQ());
    pinMode(AXP202_INT, INPUT_PULLUP);
    attachInterrupt(AXP202_INT, [] {
        irq = true;
    }, FALLING);

    
    //!Clear IRQ unprocessed  first
    ttgo->power->enableIRQ(AXP202_PEK_SHORTPRESS_IRQ, true);
    ttgo->power->clearIRQ();

    pinMode(TOUCH_INT, INPUT);
    
    esp_sleep_wakeup_cause_t wakeup_reason;
    if(esp_sleep_get_wakeup_cause() != ESP_SLEEP_WAKEUP_EXT1){
      ttgo->rtc->setDateTime(2019, 8, 21, 13, 55, 40);
      firstboot = false;
    }
}

void loop()
{
  clk();
    if (irq) {
        irq = false;
        ttgo->power->readIRQ();
        Serial.printf("|%d|\n",ttgo->power->readIRQ());
        if (ttgo->power->isPEKShortPressIRQ()) {
            // Clean power chip irq status
            ttgo->power->clearIRQ();

            // Set  touchscreen sleep
            ttgo->displaySleep();

            /*
            In TWatch2019/ Twatch2020V1, touch reset is not connected to ESP32,
            so it cannot be used. Set the touch to sleep,
            otherwise it will not be able to wake up.
            Only by turning off the power and powering on the touch again will the touch be working mode
            // ttgo->displayOff();
            */

            ttgo->powerOff();

            //Set all channel power off
            ttgo->power->setPowerOutPut(AXP202_LDO3, false);
            ttgo->power->setPowerOutPut(AXP202_LDO2, false);

            // TOUCH SCREEN  Wakeup source
            esp_sleep_enable_ext1_wakeup(GPIO_SEL_38, ESP_EXT1_WAKEUP_ALL_LOW);
            // PEK KEY  Wakeup source
            // esp_sleep_enable_ext1_wakeup(GPIO_SEL_35, ESP_EXT1_WAKEUP_ALL_LOW);
            esp_deep_sleep_start();
        }
        ttgo->power->clearIRQ();
    }
}
