#include "AquariumController.h"

#include "AqController.h"
#include "AqWebServer.h"
#ifdef useSerial
  #include <BluetoothSerial.h>
  BluetoothSerial SerialBT;
#endif



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
    Serial.begin(19200);
    SerialBT.setPin("0228");
    SerialBT.begin("ESP32 Bluetooth");
  #endif

  //connect to WiFi
  #ifdef useSerial
    Serial.printf("Connecting to %s ", ssid);
    SerialBT.printf("Connecting to %s ", ssid);
  #endif
  WiFi.setHostname("AquariumController");
  WiFi.mode(WIFI_STA);
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.begin(aqController.ssid, aqController.password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  #ifdef useSerial
    Serial.println(" CONNECTED");
    Serial.println(WiFi.localIP());
  #endif
  
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
  while (taskInterruptCounter > 0) {
    portENTER_CRITICAL(&timerMux);
      taskInterruptCounter--;
    portEXIT_CRITICAL(&timerMux);
    aqController.nextTaskWithEvent->doTask();
  }
  aqController.scheduleNextTask();
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
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
    SerialBT.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  #endif
}

