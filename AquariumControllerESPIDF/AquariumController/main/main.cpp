#include "Arduino.h"
#include "AquariumController.h"
#include "AqController.h"
#include "AqWebServer.h"
#include <WiFi.h>
#include "MailClient.h"
#include "Environment.h"

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
#ifdef ENVIRONMENT_DEV
  #include "ethernet_init.c"

  volatile SemaphoreHandle_t ethernetEventSemaphore;
  /** Event handler for Ethernet events */
  static void eth_event_handler(void *arg, esp_event_base_t event_base,
                                int32_t event_id, void *event_data)
  {
      uint8_t mac_addr[6] = {0};
      /* we can get the ethernet driver handle from event data */
      esp_eth_handle_t eth_handle = *(esp_eth_handle_t *)event_data;

      switch (event_id) {
      case ETHERNET_EVENT_CONNECTED:
          esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, mac_addr);
          ESP_LOGI(TAG, "Ethernet Link Up");
          ESP_LOGI(TAG, "Ethernet HW Addr %02x:%02x:%02x:%02x:%02x:%02x",
                  mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
          break;
      case ETHERNET_EVENT_DISCONNECTED:
          ESP_LOGI(TAG, "Ethernet Link Down");
          break;
      case ETHERNET_EVENT_START:
          ESP_LOGI(TAG, "Ethernet Started");
          break;
      case ETHERNET_EVENT_STOP:
          ESP_LOGI(TAG, "Ethernet Stopped");
          break;
      default:
          break;
      }
  }

  /** Event handler for IP_EVENT_ETH_GOT_IP */
  static void got_ip_event_handler(void *arg, esp_event_base_t event_base,
                                  int32_t event_id, void *event_data)
  {
      ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
      const esp_netif_ip_info_t *ip_info = &event->ip_info;

      ESP_LOGI(TAG, "Ethernet Got IP Address");
      ESP_LOGI(TAG, "~~~~~~~~~~~");
      ESP_LOGI(TAG, "ETHIP:" IPSTR, IP2STR(&ip_info->ip));
      ESP_LOGI(TAG, "ETHMASK:" IPSTR, IP2STR(&ip_info->netmask));
      ESP_LOGI(TAG, "ETHGW:" IPSTR, IP2STR(&ip_info->gw));
      ESP_LOGI(TAG, "~~~~~~~~~~~");
      configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
      if (getLocalTime(&timeinfo, 10000)) {
      rtc.setTimeStruct(timeinfo);
      } else {
      rtc.setTime(0, 0, 0, 1, 1, 2024);
      }
  }
#endif

extern "C" void app_main()
{
    initArduino();
    //pinMode(4, OUTPUT);
    //digitalWrite(4, HIGH);
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    #ifdef ENVIRONMENT_DEV
      //ethernetEventSemaphore = xSemaphoreCreateBinary();
      // Initialize Ethernet driver
      uint8_t eth_port_cnt = 0;
      esp_eth_handle_t *eth_handles;
      ESP_ERROR_CHECK(example_eth_init(&eth_handles, &eth_port_cnt));
      // Create instance(s) of esp-netif for Ethernet(s)
      if (eth_port_cnt == 1) {
          // Use ESP_NETIF_DEFAULT_ETH when just one Ethernet interface is used and you don't need to modify
          // default esp-netif configuration parameters.
          esp_netif_config_t cfg = ESP_NETIF_DEFAULT_ETH();
          esp_netif_t *eth_netif = esp_netif_new(&cfg);
          // Attach Ethernet driver to TCP/IP stack
          ESP_ERROR_CHECK(esp_netif_attach(eth_netif, esp_eth_new_netif_glue(eth_handles[0])));
      } else {
          // Use ESP_NETIF_INHERENT_DEFAULT_ETH when multiple Ethernet interfaces are used and so you need to modify
          // esp-netif configuration parameters for each interface (name, priority, etc.).
          esp_netif_inherent_config_t esp_netif_config = ESP_NETIF_INHERENT_DEFAULT_ETH();
          esp_netif_config_t cfg_spi = {
              .base = &esp_netif_config,
              .stack = ESP_NETIF_NETSTACK_DEFAULT_ETH
          };
          char if_key_str[10];
          char if_desc_str[10];
          char num_str[3];
          for (int i = 0; i < eth_port_cnt; i++) {
              itoa(i, num_str, 10);
              strcat(strcpy(if_key_str, "ETH_"), num_str);
              strcat(strcpy(if_desc_str, "eth"), num_str);
              esp_netif_config.if_key = if_key_str;
              esp_netif_config.if_desc = if_desc_str;
              esp_netif_config.route_prio -= i*5;
              esp_netif_t *eth_netif = esp_netif_new(&cfg_spi);

              // Attach Ethernet driver to TCP/IP stack
              ESP_ERROR_CHECK(esp_netif_attach(eth_netif, esp_eth_new_netif_glue(eth_handles[i])));
          }
      }
      // Register user defined event handers
      ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, NULL));
      ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler, NULL));

      // Start Ethernet driver state machine
      for (int i = 0; i < eth_port_cnt; i++) {
          ESP_ERROR_CHECK(esp_eth_start(eth_handles[i]));
      }
    #endif
    setup();
    EmailMessage eMessage;
    eMessage.subject = "AQ Controller Notification";
    eMessage.body = "AQ Controller Starting Up.";
    xTaskCreate(sendEmailTask, "smtp_client_task", TASK_STACK_SIZE, (void*) &eMessage, tskIDLE_PRIORITY, NULL);
    while(true) {
      loop();
    }
}

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
  
  #ifdef ENVIRONMENT_PROD
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
  #endif
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