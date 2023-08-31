#include "AquariumController.h"

#include "AqController.h"
#include "AqWebServer.h"
#ifdef useSerial
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

void setup() {
  //http://api.dynu.com/nic/update?username=cawndog&password=aqcontroller
  #ifdef useSerial
    Serial.begin(115200);
    //SerialBT.setPin("0228");
    SerialBT.begin("AqController");
  #endif
   /* static constexpr char* ssid = "Pepper";
    static constexpr char* password = "unlawfulOwl69!";
    static constexpr char* ntpServer = "pool.ntp.org";
    static const long  gmtOffset_sec = -25200;
    static const int   daylightOffset_sec = 3600;
    */
  //connect to WiFi
  /*#ifdef useSerial
    Serial.printf("Connecting to %s ", aqController.ssid);
    SerialBT.printf("Connecting to %s ", aqController.ssid);
  #endif*/
  #ifdef useSerial
    Serial.printf("Connecting to %s ", "Pepper");
    SerialBT.printf("Connecting to %s ", "Pepper");
  #endif
  /*WiFi.disconnect();
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
  /*
  aqController.init(&aqWebServer);
  aqWebServer.updateDynamicIP();

  syncSemaphore = xSemaphoreCreateBinary();
  aqController.taskTimer = timerBegin(0, 40000, true); //counter will increment 2,000 times/second
  timerAttachInterrupt(aqController.taskTimer, &taskTimerInterrupt, true);
  aqController.scheduleNextTask();*/
}

void loop() {
  /*xSemaphoreTake(syncSemaphore, portMAX_DELAY);
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
      SerialBT.println("Failed to obtain time");
    #endif
    return;
  }
  #ifdef useSerial
    Serial.println(&(aqController.timeinfo), "%A, %B %d %Y %H:%M:%S");
    SerialBT.println(&(aqController.timeinfo), "%A, %B %d %Y %H:%M:%S");
  #endif
}

