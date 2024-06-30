#include <string.h>
#include "AqWebServer.h"

// Set web server port number to 80
//WiFiServer server(80);

//const char HTML_home[] PROGMEM = "<!DOCTYPE html><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, maximum-scale=1.0, user-scalable=0\"><html> <head> <style>/* The switch - the box around the slider */ .switch{position: relative; display: inline-block; width: 60px; height: 34px;}/* Hide default HTML checkbox */ .switch input{opacity: 0; width: 0; height: 0;}/* The slider */ .slider{position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; -webkit-transition: .3s; transition: .3s;}.slider:before{position: absolute; content: \"\"; height: 26px; width: 26px; left: 4px; bottom: 4px; background-color: rgb(255, 255, 255); -webkit-transition: .3s; transition: .3s;}input:checked + .slider{background-color: #5BC236;}input:focus + .slider{box-shadow: 0 0 1px rgb(232, 232, 232);}input:checked + .slider:before{-webkit-transform: translateX(26px); -ms-transform: translateX(26px); transform: translateX(26px);}input:disabled + .slider:before{background-color: #ccc;}/* Rounded sliders */ .slider.round{border-radius: 34px;}.slider.round:before{border-radius: 50%;}.center{margin-left: auto; margin-right: auto;}.center-text{text-align: center;}.left-text{text-align: left; padding-left:5%; padding-top: 5%;}.right-text{text-align: right; padding-right:5%; padding-top: 5%;}.unselectable{-webkit-user-select: none; -webkit-touch-callout: none; -moz-user-select: none; -ms-user-select: none; user-select: none;}</style> </head> <body class=\"unselectable\" style=\"background-color: rgb(232, 232, 232);\"> <table class=\"center center-text\" style=\"table-layout: fixed; padding-top: 5%;\"> <tr> <td colspan=\"2\"> <h1 style=\"color: rgb(5, 164, 244)\">Aquarium Controller</h1> </td></tr><tr> <td style=\"width: 50%;\">Temperature<br><div id=\"tempDiv\" style=\"font-size:large;font-weight: bold; visibility: hidden;\"><span id=\"temp\"></span></div></td><td style=\"width: 50%;\">TDS<br><div id=\"tdsDiv\" style=\"font-size:large;font-weight: bold; visibility: hidden;\"><span style=\"font-size:large;font-weight: bold;\"id=\"tds\"></span></div></td></tr><tr> <td class=\"right-text\">Lights</td><td class=\"left-text\"><label class=\"switch\"> <input id=\"lights\" type=\"checkbox\" onclick=\"checkClickFunc('lights')\"> <span class=\"slider round\"></span> </label> </td></tr><tr> <td class=\"right-text\">Filter</td><td class=\"left-text\"><label class=\"switch\"> <input id=\"filter\" type=\"checkbox\" onclick=\"checkClickFunc('filter')\"> <span class=\"slider round\"></span> </label> </td></tr><tr> <td class=\"right-text\">CO2</td><td class=\"left-text\"><label class=\"switch\"> <input id=\"co2\" type=\"checkbox\" onclick=\"checkClickFunc('co2')\"> <span class=\"slider round\"></span> </label> </td></tr><tr> <td class=\"right-text\">Air Pump</td><td class=\"left-text\"><label class=\"switch\"> <input id=\"air\" type=\"checkbox\" onclick=\"checkClickFunc('air')\"> <span class=\"slider round\"></span> </label> </td></tr><tr> <td class=\"right-text\">Maintenance Mode</td><td class=\"left-text\"><label class=\"switch\"> <input id=\"maint\" type=\"checkbox\" onclick=\"checkClickFunc('maint')\"> <span class=\"slider round\"></span> </label> </td></tr><tr> <td class=\"center-text\" style=\"padding-top: 10%;\"><div id=\"heaterStateDiv\" style=\"visibility: hidden;\">Heater: <span id=\"heaterState\"></span></div></td></tr></table> <script>window.addEventListener(\"DOMContentLoaded\", (event)=>{getCurrentState(); let timer=setTimeout(function refreshState(){getCurrentState(); timer=setTimeout(refreshState, 5000);}, 5000);}); function checkClickFunc(device){var checkbox=document.getElementById(device); if (checkbox.checked==true){if (device=='air'){document.getElementById('co2').checked=false;}if (device=='co2'){document.getElementById('air').checked=false;}if (device=='maint'){filter=document.getElementById('filter'); filter.checked=false; filter.disabled=true; document.getElementById('heaterState').innerHTML=\"OFF\";}fetch('http://10.0.0.96/'+device+'On',{method: 'POST', headers:{'Content-Type': 'application/json'}, body: JSON.stringify({\"id\": 78912})}) .then (response=>{console.log(response.status);});}else{fetch('http://10.0.0.96/'+device+'Off',{method: 'POST', headers:{'Content-Type': 'application/json'}, body: JSON.stringify({\"id\": 78913})}) .then (response=>{console.log(response.status); if (response.status==200){if (device=='maint'){filter=document.getElementById('filter'); filter.checked=true; filter.disabled=false;}}});}}function getCurrentState(){fetch('http://10.0.0.96/getCurrentState') .then((response)=> response.json()) .then((json)=>{console.log(json); document.getElementById('temp').innerHTML=json.temp; document.getElementById('tds').innerHTML=json.tds; document.getElementById(\"tempDiv\").style.visibility=\"visible\"; document.getElementById(\"tdsDiv\").style.visibility=\"visible\"; document.getElementById('lights').checked=json.lights; document.getElementById('filter').checked=json.filter; document.getElementById('co2').checked=json.co2; document.getElementById('air').checked=json.air; document.getElementById('maint').checked=json.maint; if (json.heater){document.getElementById('heaterState').innerHTML=\"ON\";}else{document.getElementById('heaterState').innerHTML=\"OFF\";}document.getElementById(\"heaterStateDiv\").style.visibility=\"visible\";});}</script> </body></html>";
// Variable to store the HTTP request


//const char* http_username = "admin";
//const char* http_password = "esp32";
// allows you to set the realm of authentication Default:"Login Required"
//const char* www_realm = "Custom Auth Realm";
// the Content of the HTML response in case of Unautherized Access Default:empty
String authFailResponse = "Authentication Failed";
//String header;


//AqWebServer::AqWebServer(int port, const char* path): server(port), ws(path) {
  AqWebServer::AqWebServer() {
    this->server = new AsyncWebServer(8008);
    this->ws = new AsyncWebSocket("/ws");
  }
  void AqWebServer::init() {

  //server->on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    //if (!request->authenticate(http_username, http_password))
    //{
      //return request->requestAuthentication();
    //}
    //request->send(200, "text/plain", "Temp: " + String(aquariumTemp) + " TDS: " + String(tdsVal));
    //request->send(200, "text/html", HTML_home);
  //});

  server->on("/wv_on", HTTP_POST, [&](AsyncWebServerRequest *request) {
    bool authFailed = checkAuthorization(request);
    if (authFailed) {
      request->send(401);
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
      request->send(401);
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
    delay(15);
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
    aqController.maintMode = true;
    aqController.filter.setStateOff();
    aqController.heater.setStateOff();
  });
  server->on("/maintOff", HTTP_POST, [&](AsyncWebServerRequest *request) {
    bool authFailed = checkAuthorization(request);
    if (authFailed) {
      return;
    }
    request->send(200);
    aqController.maintMode = false;
    aqController.filter.setStateOn();
  });
  server->on("/getCurrentState", HTTP_GET, [&](AsyncWebServerRequest *request) {
    //AsyncResponseStream *response = request->beginResponseStream("application/json");
    bool authFailed = checkAuthorization(request);
    if (authFailed) {
      return;
    }

    DynamicJsonDocument body(2048);
    body["messageType"] = "StateUpdate";
    body["maintenanceMode"] = aqController.maintMode;
    for (int i = 0; i < sizeof(aqController.sensors)/sizeof(Sensor*);i++) {
      body["sensors"][i]["name"] = aqController.sensors[i]->name;
      body["sensors"][i]["value"] = aqController.sensors[i]->getValue();
    }
    for (int i = 0; i < sizeof(aqController.devices)/sizeof(Device*);i++) {
      body["devices"][i]["name"] = aqController.devices[i]->name;
      body["devices"][i]["state"] = aqController.devices[i]->getStateBool();
    }
    /*
    body["sensors"][0]["name"] = aqController.aqTemperature.name;
    body["sensors"][0]["value"] = aqController.aqTemperature.getValue();
    body["sensors"][1]["name"] = aqController.tds.name;
    body["sensors"][1]["value"] = aqController.tds.getValue();
    body["devices"][0]["name"] = aqController.lights.name;
    body["devices"][0]["state"] = aqController.lights.getStateBool();
    body["devices"][1]["name"] = aqController.airPump.name;
    body["devices"][1]["state"] = aqController.airPump.getStateBool();
    body["devices"][2]["name"] = aqController.co2.name;
    body["devices"][2]["state"] = aqController.co2.getStateBool();
    body["devices"][3]["name"] = aqController.filter.name;
    body["devices"][3]["state"] = aqController.filter.getStateBool();
    body["devices"][4]["name"] = aqController.heater.name;
    body["devices"][4]["state"] = aqController.heater.getStateBool();
    */
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
    body["messageType"] = "SettingsUpdate";
    body["aqThermostat"] = aqController.aqThermostat;
  
    for (int i = 0; aqController.tasks[i] != NULL; i++) {
      body["tasks"][i]["name"] = aqController.tasks[i]->getName();
      body["tasks"][i]["taskType"] = aqController.tasks[i]->taskTypeToString();
      body["tasks"][i]["time"] = aqController.tasks[i]->getTime();
      body["tasks"][i]["isDisabled"] = aqController.tasks[i]->getDisabled();
      if (aqController.tasks[i]->hasConnectedTask()) {
        body["tasks"][i]["connectedTask"]["name"] = aqController.tasks[i]->connectedTask->getName();
        body["tasks"][i]["connectedTask"]["taskType"] = aqController.tasks[i]->connectedTask->taskTypeToString();
        body["tasks"][i]["connectedTask"]["time"] = aqController.tasks[i]->connectedTask->getTime();
        body["tasks"][i]["connectedTask"]["isDisabled"] = aqController.tasks[i]->connectedTask->getDisabled();
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
    JsonObject body = json.as<JsonObject>();
    if (body.containsKey("aqThermostat")) {
      aqController.aqThermostat = body["aqThermostat"];
      savedState.putShort("aqThermostat", aqController.aqThermostat);
      aqController.aqTemperature.readSensor();
    }
    const int numTasksInMsg = body["tasks"].size();
    #ifdef useSerial
      Serial.printf("numTasksInMsg %d\n", numTasksInMsg);
    #endif
    Task* task = NULL;
    Task* connectedTask = NULL;
    bool isDisabled = true;
    unsigned long time = 0;
    for (int i = 0; i < numTasksInMsg; i++) {
      const char* taskName = body["tasks"][i]["name"];
      if (taskName != NULL) {
        task = aqController.getTaskByName(taskName);
        if (task != NULL) {
          const char* connectedTaskName = body["tasks"][i]["connectedTask"]["name"];
          if (connectedTaskName != NULL) {
            connectedTask = aqController.getTaskByName(connectedTaskName);
            if (connectedTask != NULL) {
              if (body["tasks"][i]["connectedTask"]["isDisabled"] == 0) {
                isDisabled = false;
              } else {
                isDisabled = true;
              }
              time = body["tasks"][i]["connectedTask"]["time"];
              connectedTask->updateSettings(isDisabled, time);
            }
          }
          isDisabled = body["tasks"][i]["isDisabled"];
          if (body["tasks"][i]["isDisabled"] == 0) {
            isDisabled = false;
          } else {
            isDisabled = true;
          }
          time = body["tasks"][i]["time"];
          task->updateSettings(isDisabled, time);
        } 
      }
    }
    aqController.scheduleNextTask();

    request->send(200, "application/text", "setSettingsState succeeded.");
    
  });
  server->addHandler(setSettingsHandler);
  setDeviceStateHandler = new AsyncCallbackJsonWebHandler("/setDeviceState", [&](AsyncWebServerRequest *request, JsonVariant &json) {
    bool authFailed = checkAuthorization(request);
    if (authFailed) {
      return;
    }
    JsonObject body = json.as<JsonObject>();
    if (body.containsKey("devices")) {
      if (body["devices"].size() > 0) {
        Device* deviceToSet = aqController.getDeviceByName(body["devices"][0]["name"]);
        if (deviceToSet != NULL) {
          if (body["devices"][0]["state"] == true) {
            deviceToSet->setStateOn();
          }
          else {
            deviceToSet->setStateOff();
          }
        }
      }
    }
    request->send(200, "text/plain", "setDeviceOn Succeeded");
    
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
      case WS_EVT_PONG:
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
    AsyncWebHeader *header = request->getHeader("Authorization");
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
/*void AqWebServer::handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {

}*/
void AqWebServer::deviceStateUpdate(Device** devices, int numDevices) {
  DynamicJsonDocument body(1024);
  body["messageType"] = "StateUpdate";
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
  body["messageType"] = "StateUpdate";
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
void AqWebServer::updateDynamicIP() {
  HTTPClient http;
  http.begin(DYNAMIC_IP_UPDATE_URL);
  http.GET();
  http.end();
}
bool AqWebServer::processAqControllerMessage(JsonVariant &json) {
  JsonObject body = json.as<JsonObject>();
  if (body.containsKey("messageType")) {
    if (body["messageType"] == "StateUpdate") {
      if (body.containsKey("devices")) {
        if (body["devices"].size() > 0) {
          Device* deviceToSet = aqController.getDeviceByName(body["devices"][0]["name"]);
          if (deviceToSet != NULL) {
            if (body["devices"][0]["state"] == true) {
              deviceToSet->setStateOn();
            }
            else {
              deviceToSet->setStateOff();
            }
          }
        }
      }
      if (body.containsKey("maintenanceMode")) {
        if (body["maintenanceMode"] == true) {
          aqController.maintMode = true;
          aqController.filter.setStateOff();
          aqController.heater.setStateOff();
        }
        else {
          aqController.maintMode = false;
          aqController.filter.setStateOn();
          aqController.aqTemperature.readSensor();
        }
      }
      if (body.containsKey("feedMode")) {
        if (body["feedMode"] == true) {
          aqController.maintMode = true;
          
          aqController.heater.setStateOff();
        }
        else {
          aqController.maintMode = false;
          aqController.filter.setStateOn();
          aqController.aqTemperature.readSensor();
        }
      }
    }
    else if (body["messageType"] == "SettingsUpdate") {
      if (body.containsKey("aqThermostat")) {
        aqController.aqThermostat = short(body["aqThermostat"]);
        savedState.putShort("aqThermostat", aqController.aqThermostat);
        aqController.aqTemperature.readSensor();
      }
      const int numTasks = body["tasks"].size();
      for (int i = 0; i < numTasks; i++) {
        Task* task = aqController.getTaskByName(body["tasks"][i]["name"]);
        if (task != NULL) {
          task->updateSettings(body["tasks"][i]["isDisabled"], body["tasks"][i]["time"]);
          if (body["tasks"][i].containsKey("connectedTask")) {
            task = aqController.getTaskByName(body["tasks"][i]["connectedTask"]["name"]);
            if (task != NULL) {
              task->updateSettings(body["tasks"][i]["connectedTask"]["isDisabled"], body["tasks"][i]["connectedTask"]["time"]);
            }
          }
        } 
      }
      aqController.scheduleNextTask();
    } else {
      return false;
    }
  } else {
    return false;
  }
  return true;

}


