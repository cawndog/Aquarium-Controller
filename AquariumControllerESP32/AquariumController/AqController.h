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
#include "ServerCode.h"


#define NUM_TASKS 20
#ifdef useSerial
  #include <BluetoothSerial.h>
  BluetoothSerial SerialBT;
#endif

class AqController {
  static const char* ssid = "Pepper";
  static const char* password = "unlawfulOwl69!";
  static const char* ntpServer = "pool.ntp.org";
  static const long  gmtOffset_sec = -25200;
  static const int   daylightOffset_sec = 3600;
  static ESP32Time rtc; //Real time clock
  static struct tm timeinfo;
  static bool maintMode = false;
  static Preferences savedState;
  static HardwareInterface hardwareInterface;

  static Device Heater("Heater");
  static Device Lights("Lights");
  static Device CO2("CO2");
  static Device AirPump("Air Pump");
  static Device Filter("Filter");
  
  static TdsSensor tds("TDS");
  static AquariumTemperatureSensor aqTemperature("Aquarium Temperature");
  static Task* tasks[NUM_TASKS];
  static void init();

};

static AquariumController aqController;


#endif