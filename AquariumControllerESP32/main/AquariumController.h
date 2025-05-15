#ifndef AQUARIUMCONTROLLER_H
#define AQUARIUMCONTROLLER_H
#define useSerial
#include "sdkconfig.h"
/*
+-------------------+----------------+-----------------------+
| Peripheral        | ESP32 Pin      | ESP32-S3 Pin          |
+-------------------+----------------+-----------------------+
| WATER_SENSOR_PIN  | GPIO34         | GPIO4                 |
| CO2_PIN           | GPIO18         | GPIO1                 |
| WATER_VALVE_R_PIN | GPIO21         | GPIO2                 |
| WATER_VALVE_Y_PIN | GPIO19         | GPIO21                |
| TDS_SENSOR_PIN    | GPIO35         | GPIO5                 |
| TEMP_SENSOR_PIN   | GPIO13         | GPIO13                |
| AIR_SRVO_PIN      | GPIO25         | GPIO15                |
| LIGHT_SRVO_PIN    | GPIO27         | GPIO17                |
| HEATER_SRVO_PIN   | GPIO33         | GPIO7                 |
| FILTER_SRVO_PIN   | GPIO26         | GPI16                 |
+-------------------+----------------+-----------------------+
*/
//Board Pin Configuration
#if CONFIG_IDF_TARGET_ESP32
//ESP32
  #define WATER_SENSOR_PIN 34
  #define CO2_PIN 18
  #define WATER_VALVE_R_PIN 21
  #define WATER_VALVE_Y_PIN 19
  #define TDS_SENSOR_PIN 35
  #define TEMP_SENSOR_PIN 13
  #define AIR_SRVO_PIN 25//Servo #1
  #define LIGHT_SRVO_PIN 27 //Servo #2
  #define HEATER_SRVO_PIN 33//Servo #3
  #define FILTER_SRVO_PIN 26 //Servo #4

  #define WATER_SENSOR_ALARM_THRESHOLD 80 //ADC reading value that will trigger an alarm state and shut off water valve. ESP32
  #define WATER_SENSOR_READING_INTERVAL 10000 //time in ms to poll water sensor
  //#define SIMULATE_SENSOR_VALS //Simulate sensor values for testing
#endif
#if CONFIG_IDF_TARGET_ESP32S3
//ESP32 S3
  #define WATER_SENSOR_PIN 4
  #define CO2_PIN 1
  #define WATER_VALVE_R_PIN 2
  #define WATER_VALVE_Y_PIN 21
  #define TDS_SENSOR_PIN 5
  #define TEMP_SENSOR_PIN 13
  #define AIR_SRVO_PIN 15//Air Servo, Servo #1
  #define LIGHT_SRVO_PIN 17 //Light Servo, Servo #2
  #define HEATER_SRVO_PIN 7//Heater Servo, Servo #3
  #define FILTER_SRVO_PIN 16 //Filter Servo, Servo #4
  #define TEMP_SENSOR_PIN 13

  #define WATER_SENSOR_ALARM_THRESHOLD 100 //ADC reading value that will trigger an alarm state and shut off water valve.
  #define WATER_SENSOR_READING_INTERVAL 10000 //time in ms to poll water sensor
#endif
#endif


