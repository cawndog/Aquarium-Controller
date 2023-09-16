#ifndef AQ_WEB_SERVER_H
#define AQ_WEB_SERVER_H

#include "AquariumController.h"
#include "AqController.h"
#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
//#include <AsyncElegantOTA.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <cstring>
#include "AqWebServerInterface.h"
#ifdef useSerial

#endif
#ifdef useSerialBT
  extern BleSerial SerialBT;
#endif
  extern AqController aqController;
class AqWebServer: public AqWebServerInterface {
  public: 
    AsyncWebServer* server;
    AsyncWebSocket* ws;
    AsyncCallbackJsonWebHandler* setSettingsHandler;
    AsyncCallbackJsonWebHandler* setDeviceStateHandler;
    //AqWebServer(int port, const char* path);
    AqWebServer();
    void init();
    ~AqWebServer();
    //void handleWebSocketMessage(void *arg, uint8_t *data, size_t len);
    //void webServerSetup();
    bool checkAuthorization(AsyncWebServerRequest *request);
    void updateDDNS();
    void deviceStateUpdate(Device** devices, int numDevices);
    void sensorReadingUpdate(Sensor* sensor);
    void updateDynamicIP();
    bool processAqControllerMessage(JsonVariant &json);

    //void updateSensorValsOnClients(Sensor* sensors[]);
    //void updateDeviceStatesOnClients(TimedDevice* devices[]);
};

class AqWebServerDummy: public AqWebServerInterface {
  public: 
    AqWebServerDummy() {};
    virtual void deviceStateUpdate(Device** devices, int numDevices) {};
    virtual void sensorReadingUpdate(Sensor* sensor) {};
    virtual void updateDynamicIP() {};
    ~AqWebServerDummy() {};
};

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