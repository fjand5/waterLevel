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

typedef struct
{
  uint32_t wakeCount;
  float lastVoltage;
  bool lastHighFloat;
  bool lastLowFloat;
  uint32_t timeStamp;
} SystemStatus;
SystemStatus *systemStatus;
bool isStatusChange = false;
void setup(void)
{
  systemStatus = (SystemStatus *)RTC_USER_MEM; // user RTC RAM area
  delay(111);
  setupFloatLevel();
  if (getBatteryVoltage() < 3.7) // bảo vệ pin
  {
    systemStatus->timeStamp += 1200e6;
    ESP.deepSleep(1200e6);
  }
  systemStatus->wakeCount += +1;

  if (abs(systemStatus->lastVoltage - getBatteryVoltage()) > 0.05)
  {
    systemStatus->lastVoltage = getBatteryVoltage();
    isStatusChange = true;
  }
  if(systemStatus->lastLowFloat != getLowLevel())  
  {
    systemStatus->lastLowFloat = getLowLevel();
    isStatusChange = true;
  }

  if(systemStatus->lastHighFloat != getHighLevel())  
  {
    systemStatus->lastHighFloat = getHighLevel();
    isStatusChange = true;
  }
  if(isStatusChange == false) // Nếu không có gì thay đổi thì ngủ tiếp 2 phút
    systemStatus->timeStamp += 120e3;
    ESP.deepSleep(120e6);

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
  if(gotTime){
    uint32_t timeStamp = timeClient.getSeconds() * 1000;
    timeStamp = timeClient.getMinutes() * 1000 * 60;
    timeStamp = timeClient.getHours() * 1000 * 60 * 24;
    systemStatus->timeStamp = timeStamp;
  }
  loopFloatLevel(true);
  loopBattery(true);
  String val = String(millis() + 1000) + " (ms)";
  String wakeCountStr = String(systemStatus->wakeCount);
  String timeStampStr = String(systemStatus->timeStamp);

  mqttClient.publish((getValue("_mqttUser") + "/waterLevelTank/workTime").c_str(), (const uint8_t *)val.c_str(), val.length(), true);
  mqttClient.publish((getValue("_mqttUser") + "/waterLevelTank/wakeCount").c_str(), (const uint8_t *)wakeCountStr.c_str(), wakeCountStr.length(), true);
  mqttClient.publish((getValue("_mqttUser") + "/waterLevelTank/timeStamp").c_str(), (const uint8_t *)timeStampStr.c_str(), timeStampStr.length(), true);

  delay(1000);

  if (!ignoreSleep && getBatteryVoltage() < 3.8) // sleep mode
  {
    systemStatus->timeStamp += 600e6;
    ESP.deepSleep(600e6);
  }
  if (!ignoreSleep && getBatteryVoltage() < 3.9) // sleep mode
  {
    systemStatus->timeStamp += 360e6;
    ESP.deepSleep(360e6);
  }
  if (!ignoreSleep && getBatteryVoltage() < 4.0) // sleep mode
  {
    systemStatus->timeStamp += 300e6;
    ESP.deepSleep(300e6);
  }
  if (!ignoreSleep && getBatteryVoltage() < 4.1) // sleep mode
  {
    systemStatus->timeStamp += 240e6;
    ESP.deepSleep(240e6);
  }
  if (!ignoreSleep && getBatteryVoltage() < 4.2) // sleep mode
  {
    systemStatus->timeStamp += 180e6;
    ESP.deepSleep(180e6);
  }
  if (!ignoreSleep && getBatteryVoltage() < 999) // sleep mode
  {
    systemStatus->timeStamp += 120e3;
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