#define BELL_PIN 2
#define APID "vocaui-bell"

#include <Arduino.h>
#include "webserver.h"
#include "wifi.h"
#include "store.h"
#include "update.h"
#include "ws.h"
#include "mqtt.h"
#include "system.h"
#include "timer.h"
#include "Ticker.h"
Ticker ringTheBellTicker;
void ringTheBell()
{
    static uint32_t count = 0;
    if (count % 100 == 0)
    {

        digitalWrite(BELL_PIN, HIGH);
    }
    else
    {
        digitalWrite(BELL_PIN, LOW);
    }
    count++;
}
void setup(void)
{
    delay(111);
    // Serial.begin(115200);
    // //Serial.println("booted");
    pinMode(BELL_PIN, OUTPUT);
    digitalWrite(BELL_PIN, LOW);

    setupStore();
    setupWifi();
    setupWebserver();
    setupUpdate();
    setupWS();
    setupSystem();
    setupMqtt();
    setupTimer();
    // //Serial.println("wait to sleep ignoreSleep");
    delay(1000);
    setOnMqttIncome([](String topic, String msg)
                    {
        if(topic == "cafevongcat/waterLevel/lowLevel"
        && msg == "0"
        ){
            ringTheBellTicker.attach_ms(100, ringTheBell);
        } 
        
               if(topic == "cafevongcat/waterLevel/lowLevel"
        && msg == "1"
        ){
            ringTheBellTicker.detach();
        } });
}
void loop(void)
{
    loopWebserver();
    loopWS();
    loopMqtt();
}