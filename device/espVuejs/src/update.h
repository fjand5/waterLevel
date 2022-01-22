/*
  To upload through terminal you can use: curl -F "image=@firmware.bin" esp8266-webupdate.local/update
*/
#pragma once
#include "webserver.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
extern ESP8266WebServer server;
ESP8266HTTPUpdateServer httpUpdater;

void setupUpdate(void) {
  httpUpdater.setup(&server);
}
