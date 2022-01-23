#define BELL_PIN 3
#define APID  "vocaui-bell"

#include <Arduino.h>
#include "webserver.h"
#include "wifi.h"
#include "store.h"
#include "update.h"
#include "ws.h"
#include "mqtt.h"
#include "system.h"
#include "timer.h"
void setup(void)
{
    delay(111);
    Serial.begin(115200);
    Serial.println("booted");
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
    Serial.println("wait to sleep ignoreSleep");
    delay(1000);
    setOnMqttIncome([](String topic, String msg)
                    {
        if(topic == "cafevongcat/waterLevel/lowLevel"
        && msg == "0"
        ){
            digitalWrite(BELL_PIN, HIGH);
        } 
        
               if(topic == "cafevongcat/waterLevel/lowLevel"
        && msg == "1"
        ){
            digitalWrite(BELL_PIN, LOW);
        } });
}
void loop(void)
{
    loopWebserver();
    loopWS();
    loopMqtt();
}