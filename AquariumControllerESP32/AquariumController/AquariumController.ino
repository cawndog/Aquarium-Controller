#include "AquariumController.h"
#include "AqController.h"

#ifdef useSerial
  #include <BluetoothSerial.h>
  BluetoothSerial SerialBT;
#endif



//-----------------------------Function Declarations-----------------------------
void printLocalTime();
void IRAM_ATTR sensorTimerInterrupt();
void IRAM_ATTR powerEventTimerInterrupt();

//-------------------------------Global Variables-------------------------------

volatile SemaphoreHandle_t syncSemaphore;
hw_timer_t* sensorTimer;
hw_timer_t* powerEventTimer;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

volatile uint8_t readSensorsInterruptCounter = 0;
volatile uint8_t powerEventInterruptCounter = 0;
volatile uint8_t tdsCounter = 0; //For timing TDS sensor readings, in seconds. This number is multiplied by 10.





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

  aqController.init();
  //AqController::init();
  //printLocalTime();
  

  //webServerSetup();        -------------WEB SERVER CODE
  //updateDDNS();
  /*syncSemaphore = xSemaphoreCreateBinary();
  sensorTimer = timerBegin(0, 40000, true); //counter will increment 2,000 times/second
  timerAttachInterrupt(sensorTimer, &sensorTimerInterrupt, true);
  timerAlarmWrite(sensorTimer, 20000, true);
  timerAlarmEnable(sensorTimer);
  powerEventTimer = timerBegin(1, 40000, true); //counter will increment 2,000 times/second
  timerAttachInterrupt(powerEventTimer, &powerEventTimerInterrupt, true);
  unsigned long currentLocalEpoch = rtc.getLocalEpoch();
  #ifdef useSerial 
    Serial.println("Setting up initial device power event timer.");
    SerialBT.println("Setting up initial device power event timer.");
  #endif
  if (timedPowerEventControl.nextDeviceWithEvent->nextEventEpochTime <= currentLocalEpoch) {

    timerAlarmWrite(powerEventTimer, 2000, true);
  }
  else {
    unsigned long secondsUntilNextEvent = 0;

    timerAlarmWrite(powerEventTimer, secondsUntilNextEvent * 2000, true);
  }
  timerAlarmEnable(powerEventTimer); 
  */
}

void loop() {
  /*
  xSemaphoreTake(syncSemaphore, portMAX_DELAY);
  //timeinfo = rtc.getTimeStruct();
  while (powerEventInterruptCounter > 0) {
    portENTER_CRITICAL(&timerMux);
      powerEventInterruptCounter--;
    portEXIT_CRITICAL(&timerMux);


    unsigned long currentLocalEpoch = rtc.getLocalEpoch();

    if (timedPowerEventControl.nextDeviceWithEvent->nextEventEpochTime <= currentLocalEpoch) {

      timerAlarmWrite(powerEventTimer, 2000, true);
    }
    else {
      unsigned long secondsUntilNextEvent = timedPowerEventControl.nextDeviceWithEvent->nextEventEpochTime - currentLocalEpoch;

      timerAlarmWrite(powerEventTimer, secondsUntilNextEvent * 2000, true);
    }
    timerAlarmEnable(powerEventTimer);
  }
  while (readSensorsInterruptCounter > 0) {
    portENTER_CRITICAL(&timerMux);
      readSensorsInterruptCounter--;
    portEXIT_CRITICAL(&timerMux);



  }*/

}
void IRAM_ATTR sensorTimerInterrupt() {
  tdsCounter++;
  readSensorsInterruptCounter++;
  xSemaphoreGiveFromISR(syncSemaphore, NULL);
  return;
}
void IRAM_ATTR powerEventTimerInterrupt() {
  powerEventInterruptCounter++;
  #ifdef useSerial
    Serial.println("In powerEventTimerInterrupt(). Power event triggered.");
    SerialBT.println("In powerEventTimerInterrupt(). Power event triggered.");
  #endif
  timerAlarmDisable(powerEventTimer);
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

