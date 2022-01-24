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
void setup(void)
{
  delay(111);
  Serial.begin(115200);
  //Serial.println("booted");

  setupStore();
  setOnStoreChange([](String id, String val, bool isChange)
                   {
                     DynamicJsonDocument doc(512);
                     JsonObject objData = doc.to<JsonObject>();
                     objData[id] = val.toFloat();
                     String ret;

                     serializeJson(objData, ret);
                     webSocket.broadcastTXT(ret);
                     mqttClient.publish((getValue("_mqttUser") + "/waterLevel/" + id).c_str(), (const uint8_t *)val.c_str(), val.length(), true); });

  //Serial.println("findMaster");

  if(findMaster()){
    ignoreSleep = true;
  }
  //Serial.print(ignoreSleep);
  //Serial.println("findMaster done");
  setupWifi();
  setupWebserver();
  setupUpdate();
  setupWS();
  setupSystem();
  setupMqtt();
  setupTimer();
  setupFloatLevel();

  loopFloatLevel();
  loopBattery();
  //Serial.println("wait to sleep ignoreSleep");
  delay(1000);
  //Serial.println("sleeped");

  if (!ignoreSleep && getBatteryVoltage() < 3.7) // sleep mode
  {
    ESP.deepSleep(120e6);
  }
  if (!ignoreSleep && getBatteryVoltage() < 3.8) // sleep mode
  {
    ESP.deepSleep(100e6);
  }
  if (!ignoreSleep && getBatteryVoltage() < 3.9) // sleep mode
  {
    ESP.deepSleep(90e6);
  }
  if (!ignoreSleep && getBatteryVoltage() < 4.0) // sleep mode
  {
    ESP.deepSleep(60e6);
  }
  if (!ignoreSleep && getBatteryVoltage() < 4.1) // sleep mode
  {
    ESP.deepSleep(30e6);
  }
  if (!ignoreSleep && getBatteryVoltage() < 4.2) // sleep mode
  {
    ESP.deepSleep(15e6);
  }
  if (!ignoreSleep && getBatteryVoltage() < 999) // sleep mode
  {
    ESP.deepSleep(10e6);
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