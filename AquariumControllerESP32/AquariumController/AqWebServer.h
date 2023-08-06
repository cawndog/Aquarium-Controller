#ifndef AQ_WEB_SERVER_H
#define AQ_WEB_SERVER_H

#include "AquariumController.h"
#include "AqController.h"
#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <cstring>
#include "AqWebServerInterface.h"
#ifdef useSerial
  #include <BluetoothSerial.h>
  extern BluetoothSerial SerialBT;
#endif

static class AqWebServer: public AqWebServerInterface {
  public: 
    AsyncWebServer server;
    AsyncWebSocket ws;

    AqWebServer();
    void handleWebSocketMessage(void *arg, uint8_t *data, size_t len);
    void webServerSetup();
    bool checkAuthorization(AsyncWebServerRequest *request);
    void updateDDNS();
    //void updateSensorValsOnClients(Sensor* sensors[]);
    //void updateDeviceStatesOnClients(TimedDevice* devices[]);
} aqWebServer;

/*struct WebSocketMessageJSON: Decodable {
  enum MessageType: String, Decodable {
    case Alert, Information, StateUpdate, Unknown
    init () {
        self = .Unknown
    };
  };
  struct Sensor: Decodable {
    var name: String
    var value: String
  };
  struct Device: Decodable {
    var name: String
    var state: Bool
  };
};*/
/*

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
*/
#endif