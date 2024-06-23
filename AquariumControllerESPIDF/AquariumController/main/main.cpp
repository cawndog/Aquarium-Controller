#include "Arduino.h"
#include "AquariumController.h"
#include "AqController.h"
#include "AqWebServer.h"
#include <WiFi.h>

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "esp_sleep.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/sens_reg.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "ulp.h"
#include "ulp_main.h"
#include "esp_adc/adc_oneshot.h"
#include "ulp/ulp_config.h"
#include "ulp_adc.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern const uint8_t ulp_main_bin_start[] asm("_binary_ulp_main_bin_start");
extern const uint8_t ulp_main_bin_end[]   asm("_binary_ulp_main_bin_end");
/* This function is called once after power-on reset, to load ULP program into
 * RTC memory and configure the ADC.
 */
static void init_ulp_program(void);

/* This function is called every time before going into deep sleep.
 * It starts the ULP program and resets measurement counter.
 */
static void start_ulp_program(void);
extern "C" void app_main()
{
  esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
  if (cause != ESP_SLEEP_WAKEUP_ULP) {
    init_ulp_program();
  }
  initArduino();
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  setup();
  start_ulp_program();
  while(true) {
    loop();
  }
  // Do your own thing
}
static void init_ulp_program(void)
{
    esp_err_t err = ulp_load_binary(0, ulp_main_bin_start,
            (ulp_main_bin_end - ulp_main_bin_start) / sizeof(uint32_t));
    ESP_ERROR_CHECK(err);

    ulp_adc_cfg_t cfg = {
        .adc_n    = (adc_unit_t)ADC_UNIT,
        .channel  = (adc_channel_t)ADC_CHANNEL,
        .atten    = (adc_atten_t)ADC_ATTEN,
        .width    = (adc_bitwidth_t)ADC_WIDTH,
        .ulp_mode = ADC_ULP_MODE_FSM,
    };

    ESP_ERROR_CHECK(ulp_adc_init(&cfg));

    ulp_high_thr = ADC_HIGH_TRESHOLD;

    /* Set ULP wake up period to 200ms */
    ulp_set_wakeup_period(0, 200000);

#if CONFIG_IDF_TARGET_ESP32
    /* Disconnect GPIO17 to remove current drain through
     * pullup/pulldown resistors on modules which have these (e.g. ESP32-WROVER)
     * GPIO12 may be pulled high to select flash voltage.
     */
    rtc_gpio_isolate(GPIO_NUM_17);
#endif // CONFIG_IDF_TARGET_ESP32

    esp_deep_sleep_disable_rom_logging(); // suppress boot messages
}
static void start_ulp_program(void)
{
    /* Reset sample counter */
    ulp_sample_counter = 0;

    /* Start the program */
    esp_err_t err = ulp_run(&ulp_entry - RTC_SLOW_MEM);
    ESP_ERROR_CHECK(err);
}
//-----------------------------Function Declarations-----------------------------
void printLocalTime();
void IRAM_ATTR taskTimerInterrupt();
void IRAM_ATTR asyncEventInterrupt();
void IRAM_ATTR waterSensorEventInterrupt();
void WiFiStationHasIP(WiFiEvent_t event, WiFiEventInfo_t info);
void WiFiStaConnectedToSoftAP(WiFiEvent_t event, WiFiEventInfo_t info);
void WiFiStationDisconnect(WiFiEvent_t event, WiFiEventInfo_t info);
void WiFiScanComplete(WiFiEvent_t event, WiFiEventInfo_t info);

//-------------------------------Global Variables-------------------------------

volatile SemaphoreHandle_t syncSemaphore;
volatile SemaphoreHandle_t waterSensorEventSemaphore;
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
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;
const char* softApSsid = SOFT_AP_SSID;
const char* softApPassword = SOFT_AP_PASSWORD;
volatile int wifiReconnectAttempts = 0;
volatile int wifiReconnectMaxAttempts = 4; //Max wifi reconnect attempts before delay attemping to connect again.
volatile int RECONNECT_DELAY = 90000;
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -25200;
const int   daylightOffset_sec = 3600;
IPAddress IP = {192, 168, 0, 2};
IPAddress gateway = {192, 168, 0, 2};
IPAddress NMask = {255, 255, 255, 0};


void setup() {
    inSetup = true;
    #ifdef useSerial
        //Serial.begin(9600);
    #endif
    #ifdef useSerialBT
        //SerialBT.setPin("0228");
        //SerialBT.begin("AqController");
    #endif
    Serial.begin(115200);

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
    asyncEventSemaphore = xSemaphoreCreateBinary();
    //asyncEventTimer = timerBegin(1, 40000, true); //counter will increment 2,000 times/second
    //timerAttachInterrupt(asyncEventTimer, &asyncEventInterrupt, true);
    //timerAlarmWrite(asyncEventTimer, 10 * 2000, false);
    //timerAlarmEnable(asyncEventTimer);
    WiFi.setAutoReconnect(false);
    WiFi.begin(ssid, password);
    xSemaphoreTake(asyncEventSemaphore, portMAX_DELAY);
    if (WiFi.status() == WL_CONNECTED) {
        configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
        if (getLocalTime(&timeinfo, 10000)) {
        rtc.setTimeStruct(timeinfo);
        } else {
        rtc.setTime(0, 0, 0, 1, 1, 2023);
        }
    } else {
        rtc.setTime(0, 0, 0, 1, 1, 2023);
    }
    savedState.begin("aqController", false);
    aqWebServer.init();
    aqController.init(&aqWebServer);
    aqWebServer.updateDynamicIP();
    syncSemaphore = xSemaphoreCreateBinary();
    //taskTimer = timerBegin(40000); //counter will increment 2,000 times/second. Arduino 2.0.1.1
    taskTimer = timerBegin(2000); //counter will increment 2,000 times/second
    timerAttachInterrupt(taskTimer, &taskTimerInterrupt);
    aqController.scheduleNextTask();
    waterSensorEventSemaphore = xSemaphoreCreateBinary();
    TaskHandle_t xHandle = NULL;
    xTaskCreate([](void* pvParameters) {
        while (true) {
        xSemaphoreTake(waterSensorEventSemaphore, portMAX_DELAY);
          Serial.printf("Water Sensor triggered interrupt. Shutting off water.\n");
          aqController.heater.setStateOff();
          aqController.filter.setStateOff();
          aqController.airPump.setStateOn();
          aqController.waterValve.setStateOff();
          int ws_val = 0;
          ws_val = analogRead(WATER_SENSOR_PIN);
          while (ws_val > 30) {
            Serial.printf("In Water Sensor Event. Water Sensor Pin Value: %d\n", ws_val);
            const TickType_t xDelay = 6000 / portTICK_PERIOD_MS;
            vTaskDelay(xDelay);
            ws_val = analogRead(WATER_SENSOR_PIN);
          }
          analogWrite(17, 0);
          pinMode(17, INPUT);
          attachInterrupt(17, waterSensorEventInterrupt, HIGH);
        }

    },"WS_Event_Task", configMINIMAL_STACK_SIZE, (void *) NULL, tskIDLE_PRIORITY, &xHandle);
    configASSERT(xHandle);
    pinMode(17, INPUT);
    attachInterrupt(17, waterSensorEventInterrupt, HIGH);
    inSetup = false;
}

void loop() {
  xSemaphoreTake(syncSemaphore, portMAX_DELAY);
  printLocalTime();

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
}

void waterSensorEventInterrupt() {
  detachInterrupt(17);
  xSemaphoreGiveFromISR(waterSensorEventSemaphore, NULL);
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
void asyncEventInterrupt() {
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
  wifiReconnectAttempts = 0;
  if (inSetup) {
    xSemaphoreGive(asyncEventSemaphore);
  } else {
    if (getLocalTime(&timeinfo, 8000)) {
      rtc.setTimeStruct(timeinfo);
    } else {
      rtc.setTime(0, 0, 0, 1, 1, 2023);
    }
    //portENTER_CRITICAL(&timerMux);
      aqController.determineTaskRunTimes();
      aqController.initSchedDeviceTasks();
      aqController.scheduleNextTask();
    //portEXIT_CRITICAL(&timerMux);
  }
  if (WiFi.getMode() == WIFI_AP_STA) {
    WiFi.softAPdisconnect();
    WiFi.mode(WIFI_STA);
  }
}
void WiFiStaConnectedToSoftAP(WiFiEvent_t event, WiFiEventInfo_t info) {
  if (inSetup) {
    xSemaphoreGive(asyncEventSemaphore);
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