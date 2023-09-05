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

//-------------------------------Global Variables-------------------------------

volatile SemaphoreHandle_t syncSemaphore;
hw_timer_t* taskTimer;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
volatile uint8_t taskInterruptCounter = 0;
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
  //AqWebServer aqWebServer(80, "/ws");
  aqWebServer.init();
  
  
  aqController.init(&aqWebServer);
  
  aqWebServer.updateDynamicIP();

  syncSemaphore = xSemaphoreCreateBinary();
  /*aqController.taskTimer = timerBegin(0, 40000, true); //counter will increment 2,000 times/second
  timerAttachInterrupt(aqController.taskTimer, &taskTimerInterrupt, true);
  aqController.scheduleNextTask();*/
}

void loop() {
  #ifdef useSerial
    //Serial.println("Hi");
    delay(1000);
  #endif
  xSemaphoreTake(syncSemaphore, portMAX_DELAY);
  /*
  //timeinfo = rtc.getTimeStruct();
  while (taskInterruptCounter > 0) {
    portENTER_CRITICAL(&timerMux);
      taskInterruptCounter--;
    portEXIT_CRITICAL(&timerMux);
    aqController.nextTaskWithEvent->doTask();
  }
  aqController.scheduleNextTask();*/
}


void IRAM_ATTR taskTimerInterrupt() {
  taskInterruptCounter++;
  #ifdef useSerial
    Serial.println("In taskTimerInterrupt(). Power event triggered.");
  #endif
  #ifdef useSerialBT
    SerialBT.println("In taskTimerInterrupt(). Power event triggered.");
  #endif
  timerAlarmDisable(aqController.taskTimer);
  xSemaphoreGiveFromISR(syncSemaphore, NULL);
}
void printLocalTime()
{ 
  if(!getLocalTime(&aqController.timeinfo)){
    #ifdef useSerial
      Serial.println("Failed to obtain time");
    #endif
    #ifdef useSerialBT
      SerialBT.println("Failed to obtain time");
    #endif
    return;
  }
  #ifdef useSerial
    Serial.println(&(aqController.timeinfo), "%A, %B %d %Y %H:%M:%S");
  #endif
  #ifdef useSerialBT
    SerialBT.println(&(aqController.timeinfo), "%A, %B %d %Y %H:%M:%S");
  #endif
}

