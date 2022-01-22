#pragma once
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include "store.h"
#include "webserver.h"
#define APID "vocaui-wl"
#define APPW "12345678"
#define MASSTER_SSID "voca-master"
void scanWifi();
void setWifi();
void getWifi();

void setupWifi(void)
{
  WiFi.mode(WIFI_AP_STA);
  if (checkKey("_apid") && checkKey("_appw"))
  {
    WiFi.softAP(getValue("_apid"), getValue("_appw"));
  }
  else
  {
    WiFi.softAP(APID, APPW);
  }
  WiFi.begin(getValue("_ssid"), getValue("_sspw"));
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED && millis() < 30000)
  {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() != WL_CONNECTED)
  {
    WiFi.mode(WIFI_AP);
    Serial.println("Only AP mode");
  }
  else
  {
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(getValue("_ssid"));
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  addHttpApi("scanWifi", scanWifi);
  addHttpApi("setWifi", setWifi);
  addHttpApi("getWifi", getWifi);
}

void loopWifi(void)
{
}

void scanWifi()
{
  DynamicJsonDocument doc(2048);
  JsonArray array = doc.to<JsonArray>();
  String ssid;
  int32_t rssi;
  uint8_t encryptionType;
  uint8_t *bssid;
  int32_t channel;
  bool hidden;
  int scanResult;

  Serial.println(F("Starting WiFi scan..."));

  scanResult = WiFi.scanNetworks(/*async=*/false, /*hidden=*/true);

  if (scanResult == 0)
  {
    Serial.println(F("No networks found"));
  }
  else if (scanResult > 0)
  {
    Serial.printf(PSTR("%d networks found:\n"), scanResult);

    // Print unsorted scan results
    for (int8_t i = 0; i < scanResult; i++)
    {
      JsonObject nestedObj = array.createNestedObject();
      WiFi.getNetworkInfo(i, ssid, encryptionType, rssi, bssid, channel, hidden);
      nestedObj["ssid"] = ssid;
      nestedObj["rssi"] = rssi;
      nestedObj["hidden"] = hidden;

      Serial.printf(PSTR("  %02d: [CH %02d] [%02X:%02X:%02X:%02X:%02X:%02X] %ddBm %c %c %s\n"),
                    i,
                    channel,
                    bssid[0], bssid[1], bssid[2],
                    bssid[3], bssid[4], bssid[5],
                    rssi,
                    (encryptionType == ENC_TYPE_NONE) ? ' ' : '*',
                    hidden ? 'H' : 'V',
                    ssid.c_str());
      delay(0);
    }
  }
  else
  {
    Serial.printf(PSTR("WiFi scan error %d"), scanResult);
  }
  String ret;
  serializeJson(array, ret);
  server.send(200, "application/json", ret.c_str());
}
bool findMaster()
{
  String ssid;
  int32_t rssi;
  uint8_t encryptionType;
  uint8_t *bssid;
  int32_t channel;
  bool hidden;
  int scanResult;
  scanResult = WiFi.scanNetworks(/*async=*/false, /*hidden=*/true);
  for (int8_t i = 0; i < scanResult; i++)
  {
    WiFi.getNetworkInfo(i, ssid, encryptionType, rssi, bssid, channel, hidden);
    if(ssid == MASSTER_SSID)
      return true;
    delay(0);
  }
  return false;
}
void setWifi()
{
  DynamicJsonDocument doc(256);
  auto err = deserializeJson(doc, server.arg(0));

  if (!err)
  {
    setValue("_ssid", doc["ssid"], true);
    setValue("_sspw", doc["sspw"], true);
  }
  server.send(200, "application/json", server.arg(0));
}
void getWifi()
{
  DynamicJsonDocument doc(256);
  JsonObject obj = doc.to<JsonObject>();
  obj["wifi"] = getValue("_ssid");
  obj["ip"] = WiFi.localIP().toString();
  String ret;
  serializeJson(obj, ret);
  server.send(200, "application/json", ret);
}