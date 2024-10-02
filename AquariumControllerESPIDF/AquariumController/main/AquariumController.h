#ifndef AQUARIUMCONTROLLER_H
#define AQUARIUMCONTROLLER_H
#define useSerial
//#define useSerialBT
//Pins
/*
ESP32
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
*/
//ESP32 S3
#define WATER_SENSOR_PIN 5
#define CO2_PIN 37
#define WATER_VALVE_R_PIN 21
#define WATER_VALVE_Y_PIN 14
#define TDS_SENSOR_PIN 6
#define TEMP_SENSOR_PIN 7
#define AIR_SRVO_PIN 10//Servo #1
#define LIGHT_SRVO_PIN 11 //Servo #2
#define HEATER_SRVO_PIN 12//Servo #3
#define FILTER_SRVO_PIN 13 //Servo #4


#define WATER_SENSOR_ALARM_THRESHOLD 150 //ADC reading value that will trigger an alarm state and shut off water valve.
#define WATER_SENSOR_READING_INTERVAL 10000 //time in ms to poll water sensor

#endif