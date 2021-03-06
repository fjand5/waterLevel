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
void sleep(uint32_t seconds){
    systemStatus->timeStamp += seconds;
    // Serial.println();
    // Serial.print("sleeped: ");

    // Serial.println(systemStatus->timeStamp);
    // Serial.print(seconds);

    ESP.deepSleep(seconds * 1e6);
}
bool isStatusChange = false;
void setup(void)
{
  systemStatus = (SystemStatus *)RTC_USER_MEM; // user RTC RAM area
  systemStatus->wakeCount += +1;
  delay(111);
  // Serial.begin(115200);
  setupFloatLevel();
  if (getBatteryVoltage() < 3.7) // bảo vệ pin
  {
    sleep(1200);
  }

  if (abs(systemStatus->lastVoltage - getBatteryVoltage()) > 0.05)
  {
    systemStatus->lastVoltage = getBatteryVoltage();
    isStatusChange = true;
  }
  if (systemStatus->lastLowFloat != getLowLevel())
  {
    systemStatus->lastLowFloat = getLowLevel();
    isStatusChange = true;
  }

  if (systemStatus->lastHighFloat != getHighLevel())
  {
    systemStatus->lastHighFloat = getHighLevel();
    isStatusChange = true;
  }
  // Nếu không có gì thay đổi thì ngủ tiếp 2 phút
  // Nhưng ngủ quá 15 lần thì nhất định phải chạy tiếp
  if (isStatusChange == false // không có gì thay đổi
   &&  (systemStatus->wakeCount % 15 != 0) // ngủ quá 15 lần
   
   ) 
  {
    sleep(120);
  }

  setupStore();
  setOnStoreChange([](String id, String val, bool isChange)
                   {
                     DynamicJsonDocument doc(512);
                     JsonObject objData = doc.to<JsonObject>();
                     objData[id] = val.toFloat();
                     String ret;

                    //  serializeJson(objData, ret);
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
  if (gotTime)
  {
    uint32_t timeStamp = timeClient.getSeconds();
    timeStamp = timeClient.getMinutes() * 60;
    timeStamp = timeClient.getHours()* 60 * 24;
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
    sleep(600);
  }
  if (!ignoreSleep && getBatteryVoltage() < 3.9) // sleep mode
  {
    sleep(360);
  }
  if (!ignoreSleep && getBatteryVoltage() < 4.0) // sleep mode
  {
    sleep(300);
  }
  if (!ignoreSleep && getBatteryVoltage() < 4.1) // sleep mode
  {
    sleep(240);
  }
  if (!ignoreSleep && getBatteryVoltage() < 4.2) // sleep mode
  {
    sleep(180);
  }
  if (!ignoreSleep && getBatteryVoltage() < 999) // sleep mode
  {
    sleep(120);
  }
}
void loop(void)
{
  loopWebserver();
  loopWS();
  loopMqtt();
  loopFloatLevel();
  loopBattery();
}