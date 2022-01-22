#include <Arduino.h>
#include "store.h"

#define HIGH_LEVEL_PIN D7
#define LOW_LEVEL_PIN D6

void setupFloatLevel()
{
    pinMode(HIGH_LEVEL_PIN, INPUT_PULLUP);
    pinMode(LOW_LEVEL_PIN, INPUT_PULLUP);
    delay(111);
    setValue("highLevel", String(digitalRead(HIGH_LEVEL_PIN)));
    setValue("lowLevel", String(digitalRead(LOW_LEVEL_PIN)));
}
uint32_t floatLevelTimer = millis();
void loopFloatLevel()
{
    if (millis() - floatLevelTimer > 1000)
    {

        setValue("highLevel", String(digitalRead(HIGH_LEVEL_PIN)));
        setValue("lowLevel", String(digitalRead(LOW_LEVEL_PIN)));
        floatLevelTimer = millis();
    }
}
