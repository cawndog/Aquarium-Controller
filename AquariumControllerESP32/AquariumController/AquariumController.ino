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
void WiFiStationHasIP(WiFiEvent_t event, WiFiEventInfo_t info);
void WiFiStationDisconnect(WiFiEvent_t event, WiFiEventInfo_t info);

//-------------------------------Global Variables-------------------------------

volatile SemaphoreHandle_t syncSemaphore;
hw_timer_t* taskTimer;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
volatile uint8_t taskInterruptCounter = 0;
ESP32Time rtc; //Real time clock
tm timeinfo;
AqController aqController;
AqWebServer aqWebServer;

void setup() {
  //http://api.dynu.com/nic/update?username=cawndog&password=aqcontroller
  #ifdef useSerial
    Serial.begin(115200);
  #endif
  #ifdef useSerialBT
    //SerialBT.setPin("0228");
    SerialBT.begin("AqController");
  #endif
    const char* ssid = "Pepper";
    const char* password = "unlawfulOwl69!";
    const char* ntpServer = "pool.ntp.org";
    const long  gmtOffset_sec = -25200;
    const int   daylightOffset_sec = 3600;
  
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
  WiFi.begin("Pepper", "unlawfulOwl69!");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  #ifdef useSerial
    Serial.println(" CONNECTED");
    Serial.println(WiFi.localIP());
  #endif
  //rtc.setTime(30, 24, 15, 17, 1, 2021);  // 17th Jan 2021 15:24:30
  //rtc.setTime(1609459200);  // 1st Jan 2021 00:00:00
  rtc.setTime(0, 0, 0, 1, 1, 2023);  // 1st Jan 2023 00:00:00
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  while (!getLocalTime(&timeinfo)){
    //Wait until the local time is determined.
  }
  rtc.setTimeStruct(timeinfo);
  //AqWebServer aqWebServer(80, "/ws");
  aqWebServer.init();
  
  
  aqController.init(&aqWebServer);
  
  aqWebServer.updateDynamicIP();

  syncSemaphore = xSemaphoreCreateBinary();
  aqController.taskTimer = timerBegin(0, 40000, true); //counter will increment 2,000 times/second
  timerAttachInterrupt(aqController.taskTimer, &taskTimerInterrupt, true);
  aqController.scheduleNextTask();
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
      Serial.printf("Task event triggered for %s\n.", aqController.nextTaskWithEvent->getName().c_str());
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
  timerAlarmDisable(aqController.taskTimer);
  xSemaphoreGiveFromISR(syncSemaphore, NULL);
}
void printLocalTime()
{ 
  if(!getLocalTime(&timeinfo)){
    #ifdef useSerial
      Serial.println("Failed to obtain time");
    #endif
    #ifdef useSerialBT
      SerialBT.println("Failed to obtain time");
    #endif
    return;
  }
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
}

void WiFiStationDisconnect(WiFiEvent_t event, WiFiEventInfo_t info) {
  #ifdef useSerial
    Serial.println("WiFiStationDisconnect() WiFi disconnected.");
  #endif
}

