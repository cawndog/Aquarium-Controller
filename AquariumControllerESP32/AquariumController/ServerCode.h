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

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len);
void webServerSetup();
bool checkAuthorization(AsyncWebServerRequest *request);
void notFound(AsyncWebServerRequest *request);
void updateDDNS();
void updateSensorValsOnClients(Sensor* sensors[]);
void updateDeviceStatesOnClients(TimedDevice* devices[]);
#endif