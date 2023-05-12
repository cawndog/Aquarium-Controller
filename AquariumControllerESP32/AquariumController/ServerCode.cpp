#include <string.h>
#include "ESPAsyncWebServer.h"
#include "ServerCode.h"
#include <HTTPClient.h>
// Set web server port number to 80
//WiFiServer server(80);
AsyncWebServer server(80);
//const char HTML_home[] PROGMEM = "<!DOCTYPE html><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, maximum-scale=1.0, user-scalable=0\"><html> <head> <style>/* The switch - the box around the slider */ .switch{position: relative; display: inline-block; width: 60px; height: 34px;}/* Hide default HTML checkbox */ .switch input{opacity: 0; width: 0; height: 0;}/* The slider */ .slider{position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; -webkit-transition: .3s; transition: .3s;}.slider:before{position: absolute; content: \"\"; height: 26px; width: 26px; left: 4px; bottom: 4px; background-color: rgb(255, 255, 255); -webkit-transition: .3s; transition: .3s;}input:checked + .slider{background-color: #5BC236;}input:focus + .slider{box-shadow: 0 0 1px rgb(232, 232, 232);}input:checked + .slider:before{-webkit-transform: translateX(26px); -ms-transform: translateX(26px); transform: translateX(26px);}input:disabled + .slider:before{background-color: #ccc;}/* Rounded sliders */ .slider.round{border-radius: 34px;}.slider.round:before{border-radius: 50%;}.center{margin-left: auto; margin-right: auto;}.center-text{text-align: center;}.left-text{text-align: left; padding-left:5%; padding-top: 5%;}.right-text{text-align: right; padding-right:5%; padding-top: 5%;}.unselectable{-webkit-user-select: none; -webkit-touch-callout: none; -moz-user-select: none; -ms-user-select: none; user-select: none;}</style> </head> <body class=\"unselectable\" style=\"background-color: rgb(232, 232, 232);\"> <table class=\"center center-text\" style=\"table-layout: fixed; padding-top: 5%;\"> <tr> <td colspan=\"2\"> <h1 style=\"color: rgb(5, 164, 244)\">Aquarium Controller</h1> </td></tr><tr> <td style=\"width: 50%;\">Temperature<br><div id=\"tempDiv\" style=\"font-size:large;font-weight: bold; visibility: hidden;\"><span id=\"temp\"></span></div></td><td style=\"width: 50%;\">TDS<br><div id=\"tdsDiv\" style=\"font-size:large;font-weight: bold; visibility: hidden;\"><span style=\"font-size:large;font-weight: bold;\"id=\"tds\"></span></div></td></tr><tr> <td class=\"right-text\">Lights</td><td class=\"left-text\"><label class=\"switch\"> <input id=\"lights\" type=\"checkbox\" onclick=\"checkClickFunc('lights')\"> <span class=\"slider round\"></span> </label> </td></tr><tr> <td class=\"right-text\">Filter</td><td class=\"left-text\"><label class=\"switch\"> <input id=\"filter\" type=\"checkbox\" onclick=\"checkClickFunc('filter')\"> <span class=\"slider round\"></span> </label> </td></tr><tr> <td class=\"right-text\">CO2</td><td class=\"left-text\"><label class=\"switch\"> <input id=\"co2\" type=\"checkbox\" onclick=\"checkClickFunc('co2')\"> <span class=\"slider round\"></span> </label> </td></tr><tr> <td class=\"right-text\">Air Pump</td><td class=\"left-text\"><label class=\"switch\"> <input id=\"air\" type=\"checkbox\" onclick=\"checkClickFunc('air')\"> <span class=\"slider round\"></span> </label> </td></tr><tr> <td class=\"right-text\">Maintenance Mode</td><td class=\"left-text\"><label class=\"switch\"> <input id=\"maint\" type=\"checkbox\" onclick=\"checkClickFunc('maint')\"> <span class=\"slider round\"></span> </label> </td></tr><tr> <td class=\"center-text\" style=\"padding-top: 10%;\"><div id=\"heaterStateDiv\" style=\"visibility: hidden;\">Heater: <span id=\"heaterState\"></span></div></td></tr></table> <script>window.addEventListener(\"DOMContentLoaded\", (event)=>{getCurrentState(); let timer=setTimeout(function refreshState(){getCurrentState(); timer=setTimeout(refreshState, 5000);}, 5000);}); function checkClickFunc(device){var checkbox=document.getElementById(device); if (checkbox.checked==true){if (device=='air'){document.getElementById('co2').checked=false;}if (device=='co2'){document.getElementById('air').checked=false;}if (device=='maint'){filter=document.getElementById('filter'); filter.checked=false; filter.disabled=true; document.getElementById('heaterState').innerHTML=\"OFF\";}fetch('http://10.0.0.96/'+device+'On',{method: 'POST', headers:{'Content-Type': 'application/json'}, body: JSON.stringify({\"id\": 78912})}) .then (response=>{console.log(response.status);});}else{fetch('http://10.0.0.96/'+device+'Off',{method: 'POST', headers:{'Content-Type': 'application/json'}, body: JSON.stringify({\"id\": 78913})}) .then (response=>{console.log(response.status); if (response.status==200){if (device=='maint'){filter=document.getElementById('filter'); filter.checked=true; filter.disabled=false;}}});}}function getCurrentState(){fetch('http://10.0.0.96/getCurrentState') .then((response)=> response.json()) .then((json)=>{console.log(json); document.getElementById('temp').innerHTML=json.temp; document.getElementById('tds').innerHTML=json.tds; document.getElementById(\"tempDiv\").style.visibility=\"visible\"; document.getElementById(\"tdsDiv\").style.visibility=\"visible\"; document.getElementById('lights').checked=json.lights; document.getElementById('filter').checked=json.filter; document.getElementById('co2').checked=json.co2; document.getElementById('air').checked=json.air; document.getElementById('maint').checked=json.maint; if (json.heater){document.getElementById('heaterState').innerHTML=\"ON\";}else{document.getElementById('heaterState').innerHTML=\"OFF\";}document.getElementById(\"heaterStateDiv\").style.visibility=\"visible\";});}</script> </body></html>";
// Variable to store the HTTP request

const char* http_username = "admin";
const char* http_password = "esp32";
// allows you to set the realm of authentication Default:"Login Required"
const char* www_realm = "Custom Auth Realm";
// the Content of the HTML response in case of Unautherized Access Default:empty
String authFailResponse = "Authentication Failed";
String header;

extern PowerControl powerControl;
extern SensorControl sensorControl;
extern bool maintMode;
extern TimedPowerEventControl timedPowerEventControl;
void webServerSetup() {

  /*server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    //if (!request->authenticate(http_username, http_password))
    //{
      //return request->requestAuthentication();
    //}
    //request->send(200, "text/plain", "Temp: " + String(aquariumTemp) + " TDS: " + String(tdsVal));
    request->send(200, "text/html", HTML_home);
  });*/
  server.on("/restart", HTTP_GET, [](AsyncWebServerRequest *request) {
    bool authFailed = checkAuthorization(request);
    if (authFailed) {
      return;
    }
    request->send(200, "text/plain", "Restarting...");
    delay(15);
    ESP.restart();
  });
  server.on("/co2On", HTTP_POST, [](AsyncWebServerRequest *request) {
    bool authFailed = checkAuthorization(request);
    if (authFailed) {
      return;
    }
    request->send(200);
    powerControl.airControl(AUXON);
  });
  server.on("/co2Off", HTTP_POST, [](AsyncWebServerRequest *request) {
    bool authFailed = checkAuthorization(request);
    if (authFailed) {
      return;
    }
    request->send(200);
    powerControl.airControl(OFF);
  });
  server.on("/airOn", HTTP_POST, [](AsyncWebServerRequest *request) {
    bool authFailed = checkAuthorization(request);
    if (authFailed) {
      return;
    }
    request->send(200);
    powerControl.airControl(ON);
  });
  server.on("/airOff", HTTP_POST, [](AsyncWebServerRequest *request) {
    bool authFailed = checkAuthorization(request);
    if (authFailed) {
      return;
    }
    request->send(200);
    powerControl.airControl(OFF);
  });
  server.on("/lightsOn", HTTP_POST, [](AsyncWebServerRequest *request) {
    bool authFailed = checkAuthorization(request);
    if (authFailed) {
      return;
    }
    request->send(200);
    powerControl.lightControl(ON);
  });
  server.on("/lightsOff", HTTP_POST, [](AsyncWebServerRequest *request) {
    bool authFailed = checkAuthorization(request);
    if (authFailed) {
      return;
    }
    request->send(200);
    powerControl.lightControl(OFF);
  });
  server.on("/heaterOff", HTTP_GET, [](AsyncWebServerRequest *request) {
    bool authFailed = checkAuthorization(request);
    if (authFailed) {
      return;
    }
    request->send(200, "text/plain", "Heater Control Off");
    powerControl.heaterControl(OFF);
  });
  server.on("/heaterOn", HTTP_GET, [](AsyncWebServerRequest *request) {
    bool authFailed = checkAuthorization(request);
    if (authFailed) {
      return;
    }
    request->send(200, "text/plain", "Heater Control On");
    powerControl.heaterControl(ON);
  });
  server.on("/filterOff", HTTP_POST, [](AsyncWebServerRequest *request) {
    bool authFailed = checkAuthorization(request);
    if (authFailed) {
      return;
    }
    request->send(200);
    powerControl.filterControl(OFF);
  });
  server.on("/filterOn", HTTP_POST, [](AsyncWebServerRequest *request) {
    bool authFailed = checkAuthorization(request);
    if (authFailed) {
      return;
    }
    if (!maintMode) {
      request->send(200);
      powerControl.filterControl(ON);
    } else {
      request->send(409);
    }
  });
  server.on("/maintOn", HTTP_POST, [](AsyncWebServerRequest *request) {
    bool authFailed = checkAuthorization(request);
    if (authFailed) {
      return;
    }
    request->send(200);
    maintMode = true;
    powerControl.filterControl(OFF);
    powerControl.heaterControl(OFF);
  });
  server.on("/maintOff", HTTP_POST, [](AsyncWebServerRequest *request) {
    bool authFailed = checkAuthorization(request);
    if (authFailed) {
      return;
    }
    request->send(200);
    maintMode = false;
    powerControl.filterControl(ON);
  });
  server.on("/getCurrentState", HTTP_GET, [](AsyncWebServerRequest *request) {
    //AsyncResponseStream *response = request->beginResponseStream("application/json");
    bool authFailed = checkAuthorization(request);
    if (authFailed) {
      return;
    }
    DynamicJsonDocument body(1024);
    char tempBuf[9];
    snprintf(tempBuf, 9, "%.1f °F", sensorControl.getAquariumTemp());
    char tdsBuf[8];
    snprintf(tdsBuf, 8, "%d PPM", sensorControl.getTdsVal());
    body["temp"] = tempBuf;
    body["tds"] = tdsBuf;
    body["lights"] = (powerControl.getLightState()==1) ? true : false;
    body["filter"] = (powerControl.getFilterState()==1) ? true : false;
    body["heater"] = (powerControl.getHeaterState()==1) ? true : false;
    body["maint"] = maintMode;
    body["co2"] = (powerControl.getAirState()==2) ? true : false;
    body["air"] = (powerControl.getAirState()==1) ? true : false;
    String response;
    serializeJson(body, response);
    
    #ifdef useSerial
      Serial.println(response);
      SerialBT.println(response);
    #endif
    request->send(200, "application/json", response);
  });

  server.on("/getSettingsState", HTTP_GET, [](AsyncWebServerRequest *request) {
    bool authFailed = checkAuthorization(request);
    if (authFailed) {
      return;
    }
    DynamicJsonDocument body(1024);
    body["aquariumThermostat"] = sensorControl.getAquariumThermostat();
    String deviceName = "";
    for (int i = 0; i < timedPowerEventControl.getNumDevices(); i++) {
      deviceName = timedPowerEventControl.getDeviceByNumber(i)->deviceName;
      body["timers"][deviceName]["onHr"] = timedPowerEventControl.getDeviceByNumber(i)->getOnHr();
      body["timers"][deviceName]["onMin"] = timedPowerEventControl.getDeviceByNumber(i)->getOnMin();
      body["timers"][deviceName]["offHr"] = timedPowerEventControl.getDeviceByNumber(i)->getOffHr();
      body["timers"][deviceName]["offMin"] = timedPowerEventControl.getDeviceByNumber(i)->getOffMin();
    }

    String response; 
    serializeJson(body, response);
    request->send(200, "application/json", response);
  });
  
  AsyncCallbackJsonWebHandler* setSettingsHandler = new AsyncCallbackJsonWebHandler("/setSettingsState", [](AsyncWebServerRequest *request, JsonVariant &json) {
    bool authFailed = checkAuthorization(request);
    if (authFailed) {
      return;
    }
    JsonObject body = json.as<JsonObject>();
    sensorControl.setAquariumThermostat(body["aquariumThermostat"]);
    String deviceName = "";
    for (int i = 0; i < timedPowerEventControl.getNumDevices(); i++) {
      deviceName = timedPowerEventControl.getDeviceByNumber(i)->deviceName;
      timedPowerEventControl.getDeviceByNumber(i)->setOnHr(body["timers"][deviceName]["onHr"]);
      timedPowerEventControl.getDeviceByNumber(i)->setOnMin(body["timers"][deviceName]["onMin"]);
      timedPowerEventControl.getDeviceByNumber(i)->setOffHr(body["timers"][deviceName]["offHr"]);
      timedPowerEventControl.getDeviceByNumber(i)->setOffMin(body["timers"][deviceName]["offMin"]);
      timedPowerEventControl.initNewSettings();
    }
    String response = "";
    serializeJson(body, response);
    #ifdef useSerial
      Serial.println(response);
      SerialBT.println(response);
    #endif
    request->send(200, "application/json", response);
    
  });
  server.addHandler(setSettingsHandler);

  server.onNotFound(notFound);  

  //AsyncElegantOTA.begin(&server);    // Start AsyncElegantOTA
  server.begin();
  #ifdef useSerial
    Serial.println("HTTP server started");
    SerialBT.println("HTTP server started");
  #endif
}
bool checkAuthorization(AsyncWebServerRequest *request) {
  bool authFailed = false;
  if (request->hasHeader("Authorization")) {
    AsyncWebHeader *header = request->getHeader("Authorization");
    char *AuthStr = strdup(header->value().c_str());
    char *savePtr;
    char *token;
    token = strtok_r(AuthStr, " ", &savePtr);
    if (token != NULL) {
      if (strcmp(token, "Bearer") == 0) {
        token = strtok_r(NULL, " ", &savePtr);
        if (token != NULL) {
          if (strcmp(token, "31f18cfbab58825aedebf9d0e14057dc") != 0) {
            authFailed = true;
          }
        } else {
          authFailed = true; 
        }
      } else { 
        authFailed = true; 
      }
    } else { 
      authFailed = true; 
    }
  } else {
    authFailed = true;
  }
  if (authFailed) {
    request->send(401, "text/plain", "Authorization Failed.");
  }
  return authFailed;
}
void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void updateDDNS() {
  HTTPClient http;
  http.begin("http://api.dynu.com/nic/update?username=cawndog&password=aqcontroller");
  http.GET();
  http.end();
}