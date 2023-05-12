#ifndef SERVER_CODE_H
#define SERVER_CODE_H

#include "AquariumController.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include "AsyncJson.h"
#include <ArduinoJson.h>
#include "PowerControl.h"
#include "SensorControl.h"
#include "TimedPowerEventControl.h"
#include <cstring>

#ifdef useSerial
  #include <BluetoothSerial.h>
  extern BluetoothSerial SerialBT;
#endif

void webServerSetup();
bool checkAuthorization(AsyncWebServerRequest *request);
void notFound(AsyncWebServerRequest *request);
void updateDDNS();

#endif