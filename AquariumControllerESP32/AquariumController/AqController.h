#ifndef AQ_CONTROLLER_H
#define AQ_CONTROLLER_H


#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include <ESP32Time.h>
#include <Preferences.h>
#include "HardwareInterface.h"
#include "Device.h"
#include "Task.h"
#include "AqWebServerInterface.h"


#define NUM_TASKS 20
#ifdef useSerial
  #include <BluetoothSerial.h>
  BluetoothSerial SerialBT;
#endif

class AqController {
  public: 
    static constexpr char* ssid = "Pepper";
    static constexpr char* password = "unlawfulOwl69!";
    static constexpr char* ntpServer = "pool.ntp.org";
    static const long  gmtOffset_sec = -25200;
    static const int   daylightOffset_sec = 3600;
    ESP32Time rtc; //Real time clock
    tm timeinfo;
    AqWebServerInterface* aqWebServerInterface;
    bool maintMode;
    Preferences savedState;
    HardwareInterface hardwareInterface;

    Device heater;
    Device lights;
    Device co2;
    Device airPump;
    Device filter;
    
    TdsSensor tds;
    AquariumTemperatureSensor aqTemperature;
    Task* tasks[NUM_TASKS];
    void init(AqWebServerInterface* aqWebServerInterface);

  };

  static AqController aqController;


#endif