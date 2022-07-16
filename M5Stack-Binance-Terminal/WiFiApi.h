#ifndef RUFFYWIFIAPI_H_
#define RUFFYWIFIAPI_H_

#include "Arduino.h"
#include <WebServer.h>
#include <WiFi.h>
#include "ArduinoJson.h"


void useWiFiApi(WebServer  *server);
String getWiFiStatus(void);
String getAvailableNetworks(bool showHidden);
void handleConnect(String body);

#endif // !INTERNALLED_H_#pragma once
