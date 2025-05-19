#include <string.h>
#include "AqWebServer.h"

String authFailResponse = "Authentication Failed";

  AqWebServer::AqWebServer() {
    this->server = new AsyncWebServer(8008);
    this->ws = new AsyncWebSocket("/ws");
  }
  void AqWebServer::init() {

  server->on("/wv_on", HTTP_POST, [&](AsyncWebServerRequest *request) {
    bool authFailed = checkAuthorization(request);
    if (authFailed) {
      return;
    }
    request->send(200);
    Device* deviceToSet = aqController.getDeviceByName("Water Valve");
    if (deviceToSet != NULL) {
      deviceToSet->setStateOn();
    }
  });
  server->on("/wv_off", HTTP_POST, [&](AsyncWebServerRequest *request) {
    bool authFailed = checkAuthorization(request);
    if (authFailed) {
      return;
    }
    request->send(200);
    Device* deviceToSet = aqController.getDeviceByName("Water Valve");
    if (deviceToSet != NULL) {
      deviceToSet->setStateOff();
    }
  });
  server->on("/restart", HTTP_GET, [&](AsyncWebServerRequest *request) {
    bool authFailed = checkAuthorization(request);
    if (authFailed) {
      return;
    }
    request->send(200, "text/plain", "Restarting...");
    const TickType_t xDelay = 1000/ portTICK_PERIOD_MS;
    vTaskDelay(xDelay);
    ESP.restart();
  });
  server->on("/esp_alv", HTTP_GET, [&](AsyncWebServerRequest *request) {
    request->send(204);
  });
  server->on("/maintOn", HTTP_POST, [&](AsyncWebServerRequest *request) {
    bool authFailed = checkAuthorization(request);
    if (authFailed) {
      return;
    }
    request->send(200);
    aqController.maintenanceMode.setValue(1);
  });
  server->on("/maintOff", HTTP_POST, [&](AsyncWebServerRequest *request) {
    bool authFailed = checkAuthorization(request);
    if (authFailed) {
      return;
    }
    request->send(200);
    aqController.maintenanceMode.setValue(0);
  });
  server->on("/getCurrentState", HTTP_GET, [&](AsyncWebServerRequest *request) {
    //AsyncResponseStream *response = request->beginResponseStream("application/json");
    bool authFailed = checkAuthorization(request);
    if (authFailed) {
      return;
    }
    DynamicJsonDocument body(2048);
    //continue writing here
    for (int i = 0; i < sizeof(aqController.sensors)/sizeof(Sensor*);i++) {
      body["sensors"][i]["name"] = aqController.sensors[i]->name;
      body["sensors"][i]["value"] = aqController.sensors[i]->getValue();
    }
    for (int i = 0; i < sizeof(aqController.devices)/sizeof(Device*);i++) {
      body["devices"][i]["name"] = aqController.devices[i]->name;
      body["devices"][i]["state"] = aqController.devices[i]->getStateBool();
    }
    String response;
    serializeJson(body, response);
    body.clear();
    #ifdef useSerial
      Serial.println(response);
    #endif
    #ifdef useSerialBT
      SerialBT.println(response);
    #endif
    request->send(200, "application/json", response);
  });

  server->on("/getSettingsState", HTTP_GET, [&](AsyncWebServerRequest *request) {
    bool authFailed = checkAuthorization(request);
    if (authFailed) {
      return;
    }

    DynamicJsonDocument body(2048);
    for (int i = 0; i < sizeof(aqController.settings)/sizeof(GeneralSetting*);i++) {
      body["settings"]["generalSettings"][i]["name"] = aqController.settings[i]->getName();
      body["settings"]["generalSettings"][i]["value"] = aqController.settings[i]->getValue();
    }
    for (int i = 0; i < sizeof(aqController.alarms)/sizeof(Alarm*);i++) {
      body["settings"]["alarms"][i]["name"] = aqController.alarms[i]->getName();
      body["settings"]["alarms"][i]["alarmState"] = aqController.alarms[i]->getAlarmState();
      body["settings"]["alarms"][i]["alarmOverride"] = aqController.alarms[i]->getAlarmOverride();
    }
    for (int i = 0; aqController.tasks[i] != NULL; i++) {
      body["settings"]["tasks"][i]["name"] = aqController.tasks[i]->getName();
      body["settings"]["tasks"][i]["taskType"] = aqController.tasks[i]->taskTypeToString();
      body["settings"]["tasks"][i]["time"] = aqController.tasks[i]->getTime();
      body["settings"]["tasks"][i]["enabled"] = aqController.tasks[i]->getEnabled();
      if (aqController.tasks[i]->hasConnectedTask()) {
        body["settings"]["tasks"][i]["connectedTask"]["name"] = aqController.tasks[i]->connectedTask->getName();
        body["settings"]["tasks"][i]["connectedTask"]["taskType"] = aqController.tasks[i]->connectedTask->taskTypeToString();
        body["settings"]["tasks"][i]["connectedTask"]["time"] = aqController.tasks[i]->connectedTask->getTime();
        body["settings"]["tasks"][i]["connectedTask"]["enabled"] = aqController.tasks[i]->connectedTask->getEnabled();
      }
    }

    String response; 
    serializeJson(body, response);
    body.clear();
    #ifdef useSerial
      Serial.printf("in /getSettingsState. Sending response: \n");
      Serial.println(response);
    #endif
    request->send(200, "application/json", response);
  });
  
  setSettingsHandler = new AsyncCallbackJsonWebHandler("/setSettingsState", [&](AsyncWebServerRequest *request, JsonVariant &json) {
    bool authFailed = checkAuthorization(request);
    if (authFailed) {
      return;
    }
    processAqControllerMessage(json);
    request->send(200, "application/text", "setSettingsState succeeded.");
    
  });
  server->addHandler(setSettingsHandler);
  setDeviceStateHandler = new AsyncCallbackJsonWebHandler("/setDeviceState", [&](AsyncWebServerRequest *request, JsonVariant &json) {
    bool authFailed = checkAuthorization(request);
    if (authFailed) {
      return;
    }
    processAqControllerMessage(json);
    request->send(200, "text/plain", "setDeviceState Succeeded");
    
  });
  server->addHandler(setDeviceStateHandler);
    sendMessageHandler = new AsyncCallbackJsonWebHandler("/sendMessage", [&](AsyncWebServerRequest *request, JsonVariant &json) {
    bool authFailed = checkAuthorization(request);
    if (authFailed) {
        return;
      }
      processAqControllerMessage(json);
      request->send(200, "text/plain", "Message Received.");
  });
  server->addHandler(sendMessageHandler);
  server->onNotFound([&](AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
  });  

  //init web socket
  ws->onEvent([&](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
    switch (type) {
      case WS_EVT_CONNECT:
        #ifdef useSerial
          Serial.printf("WebSocket client #%lu connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        #endif
        //ws->text(client->id(), "connected");
        break;
      case WS_EVT_DISCONNECT:
        #ifdef useSerial
          Serial.printf("WebSocket client #%lu disconnected\n", client->id());
        #endif
        break;
      case WS_EVT_DATA:
        //handleWebSocketMessage(arg, data, len);
        break;
      case WS_EVT_PING:
        ws->text(client->id(), "Pong");
        break;
      case WS_EVT_PONG:
        break;
      case WS_EVT_ERROR:
        break;
    }
  });
  server->addHandler(ws);
  
  //AsyncElegantOTA.begin(&server);    // Start AsyncElegantOTA
  server->begin();
  #ifdef useSerial
    Serial.println("HTTP server started");
  #endif
  #ifdef useSerialBT
    SerialBT.println("HTTP server started");
  #endif
}
bool AqWebServer::checkAuthorization(AsyncWebServerRequest *request) {
  bool authFailed = true;
  if (request->hasHeader("Authorization")) {
    const AsyncWebHeader* header = request->getHeader("Authorization");
    String authString = header->value();
    String requiredValue = API_KEY;
    if (authString != requiredValue) {
      #ifdef useSerial
        Serial.print("Auth failed. Bad auth string: ");
        Serial.println(authString);
      #endif
      request->send(401, "text/plain", "Authorization Failed.");
      authFailed = true;
    }
    else {
      authFailed = false;
      #ifdef useSerial
        Serial.println("Auth Succeeded.");
      #endif
    }
  } else {
    #ifdef useSerial
      Serial.println("Auth failed. No auth header.");
    #endif
    request->send(401, "text/plain", "Authorization Failed.");
    authFailed = true;
  }
  return authFailed;
}
AqWebServer::~AqWebServer() {
  delete this->server;
  delete this->ws;
  delete this->setSettingsHandler;
  delete this->setDeviceStateHandler;
}

void AqWebServer::deviceStateUpdate(Device** devices, int numDevices) {
  DynamicJsonDocument body(1024);
  for (int i = 0; i < numDevices; i++) {
    body["devices"][i]["name"] = devices[i]->name;
    body["devices"][i]["state"] = devices[i]->getStateBool();
  }
  String message;
  serializeJson(body, message);
  body.clear();
  #ifdef useSerial
  Serial.println("In deviceStateUpdate()");
    Serial.println(message);
  #endif
  #ifdef useSerialBT
    SerialBT.println(message);
  #endif
  ws->textAll(message);
}
void AqWebServer::sensorReadingUpdate(Sensor* sensor) {
  
  DynamicJsonDocument body(1024);
  body["sensors"][0]["name"] = sensor->name;
  body["sensors"][0]["value"] = sensor->getValue();

  String message;
  serializeJson(body, message);
  body.clear();
  #ifdef useSerial
    Serial.println("In sensorReadingUpdate()");
    Serial.println(message);
  #endif
  #ifdef useSerialBT
    SerialBT.println(message);
  #endif
  ws->textAll(message);
}
void AqWebServer::settingUpdate(GeneralSetting* setting) {
  DynamicJsonDocument body(1024);
  body["settings"]["generalSettings"][0]["name"] = setting->getName();
  body["settings"]["generalSettings"][0]["value"] = setting->getValue();
  String message;
  serializeJson(body, message);
  body.clear();
  #ifdef useSerial
    Serial.println("In settingUpdate()");
    Serial.println(message);
  #endif
  #ifdef useSerialBT
    SerialBT.println(message);
  #endif
  ws->textAll(message);
}
void AqWebServer::alarmUpdate(Alarm* alarm) {
  DynamicJsonDocument body(1024);
  body["settings"]["alarms"][0]["name"] = alarm->getName();
  body["settings"]["alarms"][0]["alarmState"] = alarm->getAlarmState();
  body["settings"]["alarms"][0]["alarmOverride"] = alarm->getAlarmOverride();
  String message;
  serializeJson(body, message);
  body.clear();
  #ifdef useSerial
    Serial.println("In alarmUpdate()");
    Serial.println(message);
  #endif
  #ifdef useSerialBT
    SerialBT.println(message);
  #endif
  ws->textAll(message);
}
void AqWebServer::updateDynamicIP() {
  HTTPClient http;
  http.begin(DYNAMIC_IP_UPDATE_URL);
  http.GET();
  http.end();
}
bool AqWebServer::processAqControllerMessage(JsonVariant &json) {
  JsonObject body = json.as<JsonObject>();
        const int numDevices = body["devices"].size();
        for (int i = 0; i < numDevices; i++) {
          Device* deviceToSet = aqController.getDeviceByName(body["devices"][i]["name"]);
          if (deviceToSet != NULL) {
            if (body["devices"][i]["state"] == true) {
              deviceToSet->setStateOn();
            }
            else {
              deviceToSet->setStateOff();
            }
          }
        }
        if (body["sensors"].size() > 0) { 
          const int numSensors = body["sensors"].size();
          for (int i = 0; i < numSensors; i++) {
            Sensor* sensorToRead = aqController.getSensorByName(body["sensors"][i]["name"]);
            if (sensorToRead != NULL) {
              sensorToRead->readSensor();
            }
          }
        }
        if (body.containsKey("settings")) {
          const int numSettings = body["settings"]["generalSettings"].size();
          for (int i = 0; i < numSettings; i++) {
            GeneralSetting* settingToUpdate = aqController.getSettingByName(body["settings"]["generalSettings"][i]["name"]);
            if (settingToUpdate != NULL) {
              settingToUpdate->setValue(body["settings"]["generalSettings"][i]["value"]);
            }
          }
          const int numAlarms = body["settings"]["alarms"].size();
          for (int i = 0; i < numAlarms; i++) {
            Alarm* alarmToUpdate = aqController.getAlarmByName(body["settings"]["alarms"][i]["name"]);
            if (alarmToUpdate != NULL) {
              if (body["settings"]["alarms"][i].containsKey("alarmOverride")){
                alarmToUpdate->setAlarmOverride(body["settings"]["alarms"][i]["alarmOverride"]);
              } 
              if(body["settings"]["alarms"][i].containsKey("alarmState")) {
                alarmToUpdate->setAlarmState(body["settings"]["alarms"][i]["alarmState"]);
              }
            }
          }
          const int numTasks = body["settings"]["tasks"].size();
          if (numTasks > 0) {
            for (int i = 0; i < numTasks; i++) {
              Task* task = aqController.getTaskByName(body["settings"]["tasks"][i]["name"]);
              if (task != NULL) {
                if (body["settings"]["tasks"][i].containsKey("connectedTask")) {
                  Task* connectedTask = aqController.getTaskByName(body["settings"]["tasks"][i]["connectedTask"]["name"]);
                  if (task != NULL) {
                    connectedTask->updateSettings(body["settings"]["tasks"][i]["connectedTask"]["enabled"], body["settings"]["tasks"][i]["connectedTask"]["time"]);
                  }
                }
                task->updateSettings(body["settings"]["tasks"][i]["enabled"], body["settings"]["tasks"][i]["time"]);
              }
            }
            //aqController.scheduleNextTask();
          }
        }
  return true;
}


