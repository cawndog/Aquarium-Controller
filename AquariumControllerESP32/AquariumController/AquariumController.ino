#include "AquariumController.h"

#include "AqController.h"
#include "AqWebServer.h"
#ifdef useSerialBT
  #include <BleSerial.h>
  BleSerial SerialBT;
#endif
#include <WiFi.h>


//-----------------------------Function Declarations-----------------------------
void printLocalTime();
void IRAM_ATTR taskTimerInterrupt();
void IRAM_ATTR asyncEventInterrupt();
void WiFiStationHasIP(WiFiEvent_t event, WiFiEventInfo_t info);
void WiFiStationDisconnect(WiFiEvent_t event, WiFiEventInfo_t info);

//-------------------------------Global Variables-------------------------------

volatile SemaphoreHandle_t syncSemaphore;
volatile SemaphoreHandle_t asyncEventSemaphore;
hw_timer_t* taskTimer;
hw_timer_t* asyncEventTimer;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
volatile uint8_t taskInterruptCounter = 0;
volatile uint8_t asyncEventCounter = 0;
volatile bool inSetup = true;
ESP32Time rtc; //Real time clock
tm timeinfo;
Preferences savedState;
AqController aqController;
AqWebServer aqWebServer;
const char* ssid = "Pepper";
const char* password = "unlawfulOwl69!";
const char* softApSsid = "AqController";
const char* softApPassword = "unlawfulOwl69!";
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -25200;
const int   daylightOffset_sec = 3600;
IPAddress IP = {10, 0, 0, 96};
IPAddress gateway = {10, 0, 0, 96};
IPAddress NMask = {255, 255, 255, 0};
void setup() {
  inSetup = true;
  //http://api.dynu.com/nic/update?username=cawndog&password=aqcontroller
  #ifdef useSerial
    Serial.begin(115200);
  #endif
  #ifdef useSerialBT
    //SerialBT.setPin("0228");
    SerialBT.begin("AqController");
  #endif

  
  //connect to WiFi
  /*#ifdef useSerial
    Serial.printf("Connecting to %s ", aqController.ssid);
    SerialBT.printf("Connecting to %s ", aqController.ssid);
  #endif*/
  #ifdef useSerial
    Serial.printf("Connecting to %s ", "Pepper");
  #endif
  #ifdef useSerialBT
    SerialBT.printf("Connecting to %s ", "Pepper");
  #endif
  WiFi.onEvent(WiFiStationHasIP, ARDUINO_EVENT_WIFI_STA_GOT_IP);
  WiFi.onEvent(WiFiStationDisconnect, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
  WiFi.disconnect(true);
  WiFi.setHostname("AquariumController");
  WiFi.mode(WIFI_STA);
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  asyncEventSemaphore = xSemaphoreCreateBinary();
  //asyncEventTimer = timerBegin(1, 40000, true); //counter will increment 2,000 times/second
  //timerAttachInterrupt(asyncEventTimer, &asyncEventInterrupt, true);
  //timerAlarmWrite(asyncEventTimer, 10 * 2000, false);
  //timerAlarmEnable(asyncEventTimer);
  WiFi.begin(ssid, password);
  const TickType_t xDelay = 10000 / portTICK_PERIOD_MS;
  xSemaphoreTake(asyncEventSemaphore, xDelay);
  if (WiFi.status() != WL_CONNECTED) {
    #ifdef useSerial
      Serial.printf("Failed to connect to WiFi network %s.\n", ssid);
      //Serial.println(WiFi.localIP());
    #endif
    WiFi.mode(WIFI_AP);
    //WiFi.mode(STA+AP);
    WiFi.softAPConfig(IP, gateway, NMask);
    WiFi.softAP("AqController", "unlawfulOwl69!");
    printf("AP IP: %s\n", WiFi.softAPIP().toString());
    rtc.setTime(0, 0, 0, 1, 1, 2023);
  } else {
    #ifdef useSerial
      Serial.println(" CONNECTED");
      Serial.println(WiFi.localIP());
    #endif
    //rtc.setTime(30, 24, 15, 17, 1, 2021);  // 17th Jan 2021 15:24:30
    //rtc.setTime(1609459200);  // 1st Jan 2021 00:00:00
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    if (getLocalTime(&timeinfo, 10000)) {
      rtc.setTimeStruct(timeinfo);
    } else {
      rtc.setTime(0, 0, 0, 1, 1, 2023);
    }
  }  
  savedState.begin("aqController", false);
  //AqWebServer aqWebServer(80, "/ws");
  aqWebServer.init();
  
  
  aqController.init(&aqWebServer);
  
  aqWebServer.updateDynamicIP();

  syncSemaphore = xSemaphoreCreateBinary();
  taskTimer = timerBegin(0, 40000, true); //counter will increment 2,000 times/second
  timerAttachInterrupt(taskTimer, &taskTimerInterrupt, true);
  aqController.scheduleNextTask();
  inSetup = false;
}

void loop() {
  xSemaphoreTake(syncSemaphore, portMAX_DELAY);
  //timeinfo = rtc.getTimeStruct();
  printLocalTime();
  while (taskInterruptCounter > 0) {
    portENTER_CRITICAL(&timerMux);
      taskInterruptCounter--;
    portEXIT_CRITICAL(&timerMux);
  }
  if (aqController.nextTaskWithEvent != NULL) {
    #ifdef useSerial
      Serial.printf("Task event triggered for %s.\n", aqController.nextTaskWithEvent->getName().c_str());
    #endif
    aqController.nextTaskWithEvent->doTask();
  }    
  aqController.scheduleNextTask();
}


void IRAM_ATTR taskTimerInterrupt() {
  taskInterruptCounter++;
  #ifdef useSerial
    Serial.println("In taskTimerInterrupt(). Task event triggered.");
  #endif
  #ifdef useSerialBT
    SerialBT.println("In taskTimerInterrupt(). Task event triggered.");
  #endif
  timerAlarmDisable(taskTimer);
  xSemaphoreGiveFromISR(syncSemaphore, NULL);
}
void IRAM_ATTR asyncEventInterrupt() {
 // timerAlarmDisable(asyncEventTimer);
  //xSemaphoreGiveFromISR(asyncEventSemaphore, NULL);
}
void printLocalTime()
{ 
  /*if(!getLocalTime(&timeinfo)){
    #ifdef useSerial
      Serial.println("Failed to obtain time");
    #endif
    #ifdef useSerialBT
      SerialBT.println("Failed to obtain time");
    #endif
    return;
  }*/
  timeinfo = rtc.getTimeStruct();
  #ifdef useSerial
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  #endif
  #ifdef useSerialBT
    SerialBT.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  #endif
}

void WiFiStationHasIP(WiFiEvent_t event, WiFiEventInfo_t info) {
  #ifdef useSerial
    Serial.println("WiFiStationHasIP() Got IP.");
  #endif
  if (inSetup) {
    xSemaphoreGive(asyncEventSemaphore);
  }
}

void WiFiStationDisconnect(WiFiEvent_t event, WiFiEventInfo_t info) {
  #ifdef useSerial
    Serial.println("WiFiStationDisconnect() WiFi disconnected.");
  #endif
  if (inSetup) {
    WiFi.begin(ssid, password);
  } else {
    WiFi.begin(ssid, password);
  }
}

