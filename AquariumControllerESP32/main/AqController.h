#ifndef AQ_CONTROLLER_H
#define AQ_CONTROLLER_H


#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include <ESP32Time.h>
#include <Preferences.h>
#include "HardwareInterface.h"
#include "Device.h"
#include "Sensor.h"
#include "Task.h"
#include "Setting.h"
#include "Alarm.h"
#include "AqWebServerInterface.h"
#include "MailClient.h"

#define NUM_TASKS 20
#ifdef useSerialBT
  #include <BleSerial.h>  
  extern BleSerial SerialBT;
#endif
extern hw_timer_t* taskTimer;
extern Preferences savedState;
class AqController {
  public: 
    const char* ntpServer = "pool.ntp.org";
    const long  gmtOffset_sec = -25200;
    const int   daylightOffset_sec = 3600;

    uint8_t feedModeOffDelay = 0;
    AqWebServerInterface* aqWebServerInterface = NULL;
    HardwareInterface hardwareInterface;
    GeneralSetting thermostat;
    GeneralSetting maintenanceMode;
    GeneralSetting feedMode;
    GeneralSetting* settings[3] = {&thermostat, &maintenanceMode, &feedMode};
    Alarm waterSensorAlarm;
    Alarm* alarms[1] = {&waterSensorAlarm};
    Device heater;
    Device lights;
    Device co2;
    Device airPump;
    Device filter;
    Device waterValve;
    Device* devices[6] = {&heater, &lights, &co2, &airPump, &filter, &waterValve};
    TdsSensor tds;
    AquariumTemperatureSensor aqTemperature;
    Sensor* sensors[2] = {&aqTemperature, &tds};
    WaterSensor waterSensor;

    short aqThermostat;
    Task* tasks[NUM_TASKS];
    Task* nextTaskWithEvent;
    AqController();
    void init(AqWebServerInterface* aqWebServerInterface);
    Task* getTaskByName(String name);
    void determineTaskRunTimes();
    
    void initSchedDeviceTasks();
    void setNextTaskWithEvent();
    void scheduleNextTask();
    Device* getDeviceByName(String devName);
    Sensor* getSensorByName(String sensorName);
    Alarm* getAlarmByName(String alarmName);
    GeneralSetting* getSettingByName(String settingName);
    void setAqWebServerInterface(AqWebServerInterface* aqWebServerInterface);
  };


#endif