#define TANK_BOARD
#include <Arduino.h>
#include "webserver.h"
#include "wifi.h"
#include "store.h"
#include "update.h"
#include "ws.h"
#include "mqtt.h"
#include "system.h"
#include "timer.h"
#include "battery.h"
#include "floatLevel.h"
bool ignoreSleep = false;

uint32_t *wakeCount;
void setup(void)
{
  wakeCount = (uint32_t *)RTC_USER_MEM; // user RTC RAM area
  wakeCount ++;
  delay(111);
  if (getBatteryVoltage() < 3.7) // sleep mode
  {
    ESP.deepSleep(1200e6);
  }
  setupStore();
  setOnStoreChange([](String id, String val, bool isChange)
                   {
                     DynamicJsonDocument doc(512);
                     JsonObject objData = doc.to<JsonObject>();
                     objData[id] = val.toFloat();
                     String ret;

                     serializeJson(objData, ret);
                     webSocket.broadcastTXT(ret);
                     mqttClient.publish((getValue("_mqttUser") + "/waterLevelTank/" + id).c_str(), (const uint8_t *)val.c_str(), val.length(), true); });

  if (findMaster())
  {
    ignoreSleep = true;
  }
  setupWifi();
  setupWebserver();
  setupUpdate();
  setupWS();
  setupSystem();

  setupMqtt();
  setupTimer();

  setupFloatLevel();
  loopFloatLevel(true);
  loopBattery(true);
  String val = String(millis() + 1000);
  String wakeCountStr = String((*wakeCount));
  mqttClient.publish((getValue("_mqttUser") + "/waterLevelTank/workTime").c_str(), (const uint8_t *)val.c_str(), val.length(), true);
  mqttClient.publish((getValue("_mqttUser") + "/waterLevelTank/wakeCount").c_str(), (const uint8_t *)wakeCountStr.c_str(), wakeCountStr.length(), true);

  delay(1000);

  if (!ignoreSleep && getBatteryVoltage() < 3.8) // sleep mode
  {
    ESP.deepSleep(600e6);
  }
  if (!ignoreSleep && getBatteryVoltage() < 3.9) // sleep mode
  {
    ESP.deepSleep(360e6);
  }
  if (!ignoreSleep && getBatteryVoltage() < 4.0) // sleep mode
  {
    ESP.deepSleep(300e6);
  }
  if (!ignoreSleep && getBatteryVoltage() < 4.1) // sleep mode
  {
    ESP.deepSleep(240e6);
  }
  if (!ignoreSleep && getBatteryVoltage() < 4.2) // sleep mode
  {
    ESP.deepSleep(180e6);
  }
  if (!ignoreSleep && getBatteryVoltage() < 999) // sleep mode
  {
    ESP.deepSleep(120e6);
  }

  // setOnMqttIncome([](String topic, String msg){
  //   //Serial.println(topic + ": "+ msg);
  //   DynamicJsonDocument doc(512);
  //   auto error = deserializeJson(doc, msg);
  //   if(error)
  //     return;
  //   JsonObject objData = doc.as<JsonObject>();
  //   String id = objData["id"];
  //   if(id.startsWith("schedule"))
  //     setValue(id, objData["value"], true);
  //   else
  //     setValue(id, objData["value"]);

  // });
  // setOnWSTextIncome([](String msg){
  //   //Serial.println(msg);
  //   DynamicJsonDocument doc(512);
  //   auto error = deserializeJson(doc, msg);
  //   if(error)
  //     return;
  //   JsonObject objData = doc.as<JsonObject>();
  //   String id = objData["id"];
  //     setValue(id, objData["value"], true);
  // });
}
void loop(void)
{
  loopWebserver();
  loopWS();
  loopMqtt();
  loopFloatLevel();
  loopBattery();
}