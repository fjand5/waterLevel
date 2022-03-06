#include <Arduino.h>
#include "store.h"

#define HIGH_LEVEL_PIN D7
#define LOW_LEVEL_PIN D6

// có sự thay đổi thì trả về true
bool setupFloatLevel()
{
    bool ret;
    pinMode(HIGH_LEVEL_PIN, INPUT_PULLUP);
    pinMode(LOW_LEVEL_PIN, INPUT_PULLUP);
    delay(111);
    if (getValue("highLevel") == String(digitalRead(HIGH_LEVEL_PIN)) && getValue("lowLevel") == String(digitalRead(LOW_LEVEL_PIN)))
    {
        ret = false;
    }
    else
    {
        ret = true;
    }
    return ret;
}
uint32_t floatLevelTimer = millis();
void loopFloatLevel(bool now = false)
{
    if (millis() - floatLevelTimer > 1000 || now)
    {

        setValue("highLevel", String(digitalRead(HIGH_LEVEL_PIN)));
        setValue("lowLevel", String(digitalRead(LOW_LEVEL_PIN)));
        floatLevelTimer = millis();
    }
}
bool getLowLevel()
{
    return digitalRead(LOW_LEVEL_PIN);
}
bool getHighLevel()
{
    return digitalRead(HIGH_LEVEL_PIN);
}
