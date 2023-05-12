#include "AquariumController.h"

#include <WiFi.h>
#include "time.h"
#include "ServerCode.h"
#include "PowerControl.h"
#include <ESP32Time.h>
#include "SensorControl.h"
#include "TimedPowerEventControl.h"

#ifdef useSerial
  #include <BluetoothSerial.h>
  BluetoothSerial SerialBT;
#endif


// Replace with your network credentials
const char* ssid = "Pepper";
const char* password = "unlawfulOwl69!";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -25200;
const int   daylightOffset_sec = 3600;


//-----------------------------Function Declarations-----------------------------
void printLocalTime();
void IRAM_ATTR sensorTimerInterrupt();
void IRAM_ATTR powerEventTimerInterrupt();

//-------------------------------Global Variables-------------------------------

volatile SemaphoreHandle_t syncSemaphore;
hw_timer_t* sensorTimer;
hw_timer_t* powerEventTimer;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
ESP32Time rtc; //Real time clock
struct tm timeinfo;
bool maintMode = false;
volatile uint8_t readSensorsInterruptCounter = 0;
volatile uint8_t powerEventInterruptCounter = 0;
volatile uint8_t tdsCounter = 0; //For timing TDS sensor readings, in seconds. This number is multiplied by 10.
PowerControl powerControl;
SensorControl sensorControl;
TimedPowerEventControl timedPowerEventControl;

void setup() {
  //http://api.dynu.com/nic/update?username=cawndog&password=aqcontroller
  #ifdef useSerial
    Serial.begin(19200);
    SerialBT.setPin("0228");
    SerialBT.begin("ESP32 Bluetooth");
  #endif
  EEPROM.begin(EEPROM_SIZE);
  /*
  EEPROM.write(AirOnHrLoc, 20);
  EEPROM.write(AirOnMinLoc, 30);
  EEPROM.write(AirOffHrLoc, 9);
  EEPROM.write(AirOffMinLoc,30);
  EEPROM.commit();
  EEPROM.write(Co2OnHrLoc, 9);
  EEPROM.write(Co2OnMinLoc, 45);
  EEPROM.write(Co2OffHrLoc, 20);
  EEPROM.write(Co2OffMinLoc, 15);
  EEPROM.commit();
  EEPROM.write(LightsOnHrLoc, 10);
  EEPROM.write(LightsOnMinLoc, 10);
  EEPROM.write(LightsOffHrLoc, 14);
  EEPROM.write(LightsOffMinLoc, 0);
  EEPROM.commit();
  */
  //connect to WiFi
  #ifdef useSerial
    Serial.printf("Connecting to %s ", ssid);
    SerialBT.printf("Connecting to %s ", ssid);
  #endif
  WiFi.setHostname("AquariumController");
  WiFi.mode(WIFI_STA);
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    #ifdef useSerial
      Serial.print(".");
      SerialBT.print(".");
    #endif
  }
  #ifdef useSerial
    Serial.println(" CONNECTED");
    Serial.println(WiFi.localIP());
    SerialBT.println(" CONNECTED");
    SerialBT.println(WiFi.localIP());
  #endif
  //Setup real time clock
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  while (!getLocalTime(&timeinfo)){
    //Wait until the local time is determined.
  }
  rtc.setTimeStruct(timeinfo); 
  printLocalTime();

  powerControl.init();
  sensorControl.init();
  timedPowerEventControl.init(&rtc, &powerControl);
  webServerSetup();
  updateDDNS();
  syncSemaphore = xSemaphoreCreateBinary();
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
    #ifdef useSerial 
      Serial.println("Next device with event had an event epoch time that occurs now or in the past. Triggering timer in 1 second.");
      SerialBT.println("Next device with event had an event epoch time that occurs now or in the past. Triggering timer in 1 second.");
    #endif
    timerAlarmWrite(powerEventTimer, 2000, true);
  }
  else {
    unsigned long secondsUntilNextEvent = timedPowerEventControl.nextDeviceWithEvent->nextEventEpochTime - currentLocalEpoch;
    #ifdef useSerial 
      Serial.print("Seconds until next event: ");
      Serial.println(secondsUntilNextEvent);
      Serial.print("Ticks until next event: ");
      Serial.println(secondsUntilNextEvent*2000);
      SerialBT.print("Seconds until next event: ");
      SerialBT.println(secondsUntilNextEvent);
      SerialBT.print("Ticks until next event: ");
      SerialBT.println(secondsUntilNextEvent*2000);
    #endif
    timerAlarmWrite(powerEventTimer, secondsUntilNextEvent * 2000, true);
  }
  timerAlarmEnable(powerEventTimer); 
}
void loop() {
  xSemaphoreTake(syncSemaphore, portMAX_DELAY);
  //timeinfo = rtc.getTimeStruct();
  while (powerEventInterruptCounter > 0) {
    portENTER_CRITICAL(&timerMux);
      powerEventInterruptCounter--;
    portEXIT_CRITICAL(&timerMux);
    #ifdef useSerial
      Serial.println("In main loop(). Power event triggered");
      SerialBT.println("In main loop(). Power event triggered");
    #endif
    timedPowerEventControl.handleEventTrigger();
    unsigned long currentLocalEpoch = rtc.getLocalEpoch();
  #ifdef useSerial 
    Serial.println("Setting up next device power event timer.");
    SerialBT.println("Setting up next device power event timer.");
  #endif
    if (timedPowerEventControl.nextDeviceWithEvent->nextEventEpochTime <= currentLocalEpoch) {
      #ifdef useSerial 
        Serial.println("Next device with event has an event epoch time that occurs now or in the past. Triggering timer in 1 second.");
        SerialBT.println("Next device with event has an event epoch time that occurs now or in the past. Triggering timer in 1 second.");
      #endif
      timerAlarmWrite(powerEventTimer, 2000, true);
    }
    else {
      unsigned long secondsUntilNextEvent = timedPowerEventControl.nextDeviceWithEvent->nextEventEpochTime - currentLocalEpoch;
      #ifdef useSerial 
        Serial.print("Seconds until next event: ");
        Serial.println(secondsUntilNextEvent);
        Serial.print("Ticks until next event: ");
        Serial.println(secondsUntilNextEvent*2000);
        SerialBT.print("Seconds until next event: ");
        SerialBT.println(secondsUntilNextEvent);
        SerialBT.print("Ticks until next event: ");
        SerialBT.println(secondsUntilNextEvent*2000);
      #endif
      timerAlarmWrite(powerEventTimer, secondsUntilNextEvent * 2000, true);
    }
    timerAlarmEnable(powerEventTimer);
  }
  while (readSensorsInterruptCounter > 0) {
    portENTER_CRITICAL(&timerMux);
      readSensorsInterruptCounter--;
    portEXIT_CRITICAL(&timerMux);
    sensorControl.readAquariumTemp();
    if (sensorControl.getAquariumTemp() > sensorControl.getAquariumThermostat() + .5) { 
      if (powerControl.getHeaterState() != OFF) {
        powerControl.heaterControl(OFF);
      }
      return;
    }
    if (sensorControl.getAquariumTemp() < sensorControl.getAquariumThermostat() - .5) {
      if (powerControl.getHeaterState() != ON) {
        if (!maintMode)
          powerControl.heaterControl(ON);
      }
    }
    if (tdsCounter > 119) {
      sensorControl.readTds(sensorControl.getAquariumTempC());
      updateDDNS();
      tdsCounter = 0;
    }
    #ifdef useSerial
      Serial.print("Temp: ");
      Serial.println(sensorControl.getAquariumTemp());
      Serial.print("TDS: ");
      Serial.println(sensorControl.getTdsValFloat());
      Serial.println("");
      Serial.print("Heater State: ");
      Serial.println(powerControl.getHeaterState());
      Serial.print("Air State: ");
      Serial.println(powerControl.getAirState());
      Serial.print("Filter State: ");
      Serial.println(powerControl.getFilterState());
      Serial.print("Light State: ");
      Serial.println(powerControl.getLightState());
      Serial.println("----------------");

      SerialBT.print("Temp: ");
      SerialBT.println(sensorControl.getAquariumTemp());
      SerialBT.print("TDS: ");
      SerialBT.println(sensorControl.getTdsValFloat());
      SerialBT.println("");
      SerialBT.print("Heater State: ");
      SerialBT.println(powerControl.getHeaterState());
      SerialBT.print("Air State: ");
      SerialBT.println(powerControl.getAirState());
      SerialBT.print("Filter State: ");
      SerialBT.println(powerControl.getFilterState());
      SerialBT.print("Light State: ");
      SerialBT.println(powerControl.getLightState());
      SerialBT.println("----------------");
    #endif
  }

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
  
  if(!getLocalTime(&timeinfo)){
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

