#include "Arduino.h"
#include "AquariumController.h"
#include "AqController.h"
#include "AqWebServer.h"
#include <WiFi.h>
#include "MailClient.h"

extern "C" void app_main()
{
    initArduino();
    //pinMode(4, OUTPUT);
    //digitalWrite(4, HIGH);
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    setup();
    EmailMessage eMessage;
    eMessage.subject = "AQ Controller Notification.";
    eMessage.body = "AQ Controller Starting Up.";
    xTaskCreate(sendEmailTask, "smtp_client_task", TASK_STACK_SIZE, (void*) &eMessage, tskIDLE_PRIORITY, NULL);
    while(true) {
      loop();
    }
}
//-----------------------------Function Declarations-----------------------------
void printLocalTime();
void IRAM_ATTR taskTimerInterrupt();
void WiFiStationHasIP(WiFiEvent_t event, WiFiEventInfo_t info);
void WiFiStaConnectedToSoftAP(WiFiEvent_t event, WiFiEventInfo_t info);
void WiFiStationDisconnect(WiFiEvent_t event, WiFiEventInfo_t info);
void WiFiScanComplete(WiFiEvent_t event, WiFiEventInfo_t info);

//-------------------------------Global Variables-------------------------------

volatile SemaphoreHandle_t syncSemaphore;
volatile SemaphoreHandle_t wifiEventSemaphore;
hw_timer_t* taskTimer;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
volatile uint8_t taskInterruptCounter = 0;
volatile bool inSetup = true;
ESP32Time rtc; //Real time clock
tm timeinfo;
Preferences savedState;
AqController aqController;
AqWebServer aqWebServer;
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;
const char* softApSsid = SOFT_AP_SSID;
const char* softApPassword = SOFT_AP_PASSWORD;
volatile int wifiReconnectAttempts = 0;
volatile int wifiReconnectMaxAttempts = 4; //Max WiFi reconnect attempts before setting up SoftAP and hosting AqController Wifi Network.
volatile int RECONNECT_DELAY = 90000; //Interval to attempt connecting to WiFi when in SoftAP mode.
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -25200;
const int   daylightOffset_sec = 3600;
IPAddress IP = {192, 168, 0, 2};
IPAddress gateway = {192, 168, 0, 2};
IPAddress NMask = {255, 255, 255, 0};


void setup() {
  Serial.begin(115200);
  inSetup = true;
  #ifdef useSerial
      //Serial.begin(9600);
  #endif
  #ifdef useSerialBT
      //SerialBT.setPin("0228");
      //SerialBT.begin("AqController");
  #endif
  

  #ifdef useSerial
      Serial.printf("Connecting to %s ", WIFI_SSID);
  #endif
  #ifdef useSerialBT
      SerialBT.printf("Connecting to %s ", WIFI_SSID);
  #endif
  WiFi.onEvent(WiFiStationHasIP, ARDUINO_EVENT_WIFI_STA_GOT_IP);
  WiFi.onEvent(WiFiStationDisconnect, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
  WiFi.onEvent(WiFiScanComplete, ARDUINO_EVENT_WIFI_SCAN_DONE);
  WiFi.onEvent(WiFiStaConnectedToSoftAP, ARDUINO_EVENT_WIFI_AP_STACONNECTED);
  WiFi.disconnect(true);
  WiFi.setHostname("AquariumController");
  WiFi.mode(WIFI_STA);
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  wifiEventSemaphore = xSemaphoreCreateBinary();
  WiFi.setAutoReconnect(false);
  WiFi.begin(ssid, password);
  xSemaphoreTake(wifiEventSemaphore, portMAX_DELAY);
  if (WiFi.status() == WL_CONNECTED) {
      configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
      if (getLocalTime(&timeinfo, 10000)) {
      rtc.setTimeStruct(timeinfo);
      } else {
      rtc.setTime(0, 0, 0, 1, 1, 2024);
      }
  } else {
      rtc.setTime(0, 0, 0, 1, 1, 2024);
  }
  savedState.begin("aqController", false);
  aqWebServer.init();
  aqController.init(&aqWebServer);
  aqWebServer.updateDynamicIP();
  initMailClient();
  syncSemaphore = xSemaphoreCreateBinary();
  //taskTimer = timerBegin(2000); //counter will increment 2,000 times/second
  //timerAttachInterrupt(taskTimer, &taskTimerInterrupt);
  //aqController.scheduleNextTask();

  pinMode(WATER_SENSOR_PIN, INPUT);
  TaskHandle_t xHandle = NULL;
  xTaskCreate([](void* pvParameters) {
    EmailMessage eMessage;
    eMessage.subject = "Aquarium Alert";
    eMessage.body = "Water Sensors have detected water.";
    while (true) {
      aqController.waterSensor.readSensor();
      Serial.printf("Water Sensor Value: %d\n", aqController.waterSensor.getValueInt());
      while (aqController.waterSensor.getValueInt() > WATER_SENSOR_ALARM_THRESHOLD) {
        Serial.printf("****** Water Sensor Alarm ******\nShutting off water.\n");
        if (aqController.waterSensorAlarm.getAlarmOverride() == false) {
          if (aqController.waterSensorAlarm.getAlarmState() > 0) {
            //If we are already in an active alarm, set alarm state to its same value.
            aqController.waterSensorAlarm.setAlarmState(aqController.waterSensorAlarm.getAlarmState());
          } else {
            //Set alarm state to current Epoch time to record the time the alarm occured.
            aqController.waterSensorAlarm.setAlarmState(rtc.getLocalEpoch());
          }
        }
        //xTaskCreate(&smtp_client_task, "smtp_client_task", TASK_STACK_SIZE, NULL, 5, NULL);
        //char* message = "Water Sensor Alarm is in an active alarm state.";
        xTaskCreate(sendEmailTask, "smtp_client_task", TASK_STACK_SIZE, (void*)&eMessage, tskIDLE_PRIORITY, NULL);
        const TickType_t xDelay = 60000 / portTICK_PERIOD_MS;
        vTaskDelay(xDelay);
        aqController.waterSensor.readSensor();
        Serial.printf("Water Sensor Value: %d\n", aqController.waterSensor.getValueInt());
      }
      Serial.printf("WS_READ high water mark %d\n", uxTaskGetStackHighWaterMark(NULL));
      const TickType_t xDelay = WATER_SENSOR_READING_INTERVAL / portTICK_PERIOD_MS;
      vTaskDelay(xDelay);
    }
  },"WS_READ", 2500, (void *) NULL, tskIDLE_PRIORITY, &xHandle);
  //configASSERT(xHandle);
  //Serial.printf("*************End Setup()*************\n");
  //Serial.printf("\nTotal heap: %lu\n", ESP.getHeapSize());
  //Serial.printf("Free heap: %lu\n", ESP.getFreeHeap());
  //Serial.printf("Total PSRAM: %lu\n", ESP.getPsramSize());
  //Serial.printf("Free PSRAM: %lu\n", ESP.getFreePsram());
  inSetup = false;
}

void loop() {
  xSemaphoreTake(syncSemaphore, portMAX_DELAY);
  printLocalTime();
  /*
  portENTER_CRITICAL(&timerMux);
    while (taskInterruptCounter > 0) {
    taskInterruptCounter--;
    }
  portEXIT_CRITICAL(&timerMux);

  if (aqController.nextTaskWithEvent != NULL) {
    #ifdef useSerial
    Serial.printf("Task event triggered for %s.\n", aqController.nextTaskWithEvent->getName().c_str());
    #endif
    aqController.nextTaskWithEvent->doTask();
  }    
  aqController.scheduleNextTask();
  */
}
void taskTimerInterrupt() {
    taskInterruptCounter++;
    #ifdef useSerial
    Serial.println("In taskTimerInterrupt(). Task event triggered.");
    #endif
    #ifdef useSerialBT
    SerialBT.println("In taskTimerInterrupt(). Task event triggered.");
    #endif
    xSemaphoreGiveFromISR(syncSemaphore, NULL);
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
  wifiReconnectAttempts = 0;
  if (inSetup) {
    xSemaphoreGive(wifiEventSemaphore);
  } else {
    if (getLocalTime(&timeinfo, 8000)) {
      rtc.setTimeStruct(timeinfo);
    } else {
      rtc.setTime(0, 0, 0, 1, 1, 2023);
    }
    //portENTER_CRITICAL(&timerMux);
      //aqController.determineTaskRunTimes();
      //aqController.initSchedDeviceTasks();
      //aqController.scheduleNextTask();
    //portEXIT_CRITICAL(&timerMux);
  }
  if (WiFi.getMode() == WIFI_AP_STA) {
    WiFi.softAPdisconnect();
    WiFi.mode(WIFI_STA);
  }
}
void WiFiStaConnectedToSoftAP(WiFiEvent_t event, WiFiEventInfo_t info) {
  if (inSetup) {
    xSemaphoreGive(wifiEventSemaphore);
  }
}
void WiFiStationDisconnect(WiFiEvent_t event, WiFiEventInfo_t info) {
  #ifdef useSerial
    Serial.printf("WiFiStationDisconnect() WiFi disconnected. ReconnectAttempts: %d\n", wifiReconnectAttempts);
  #endif
  if (wifiReconnectAttempts < wifiReconnectMaxAttempts) {
      #ifdef useSerial
    Serial.printf("wifiReconnectAttempts < wifiReconnectMaxAttempt\n");
    #endif
    wifiReconnectAttempts++;
    //WiFi.begin(ssid, password);
    WiFi.reconnect();
  } 
  else {
    Serial.printf("wifiReconnectAttempts NOT < wifiReconnectMaxAttempt. Will not attempt to reconnect.\n");
    //WiFi.mode(WIFI_AP);
    if (WiFi.getMode() != WIFI_AP_STA) {
      Serial.printf("Setting WIFI mode to WIFI_MODE_APSTA\n");
      WiFi.mode(WIFI_AP_STA);
      WiFi.softAPConfig(IP, gateway, NMask);
      WiFi.softAP(softApSsid, softApPassword);
    }
    //WiFi.scanNetworks(async, show_hidden)
    //WiFi.disconnect();
    const TickType_t xDelay = RECONNECT_DELAY / portTICK_PERIOD_MS;
    vTaskDelay(xDelay);
    Serial.printf("Scanning networks\n");
    WiFi.scanNetworks(true);
    //WiFi.scanNetworks(true, false, true, 300, 0, nullptr, nullptr);
  }
  /*if (inSetup) {
    WiFi.begin(ssid, password);
  } else {
    WiFi.begin(ssid, password);
  }*/
}
void WiFiScanComplete(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.printf("DEBUG: in WiFiScanComplete.\n");
  //const TickType_t xDelay = 60000 / portTICK_PERIOD_MS;
  
  int n = WiFi.scanComplete();
  if (n > 0) {
    Serial.printf("Networks found: %d. Searching for ssid %s.\n", n, ssid);
    for (int i = 0; i < n; i++) {
      Serial.printf("Network: %s\n", WiFi.SSID(i).c_str());
      if (strcmp(WiFi.SSID(i).c_str(), ssid) == 0) {
        Serial.printf("Network Pepper found.\n");
        WiFi.scanDelete();
        wifiReconnectAttempts = 0;
        WiFi.begin(ssid, password);
        return;
      }
    }
  }
  const TickType_t xDelay = RECONNECT_DELAY / portTICK_PERIOD_MS;
  WiFi.scanDelete();
  vTaskDelay(xDelay);
  WiFi.scanNetworks(true);
}