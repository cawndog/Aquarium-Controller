#ifndef AQUARIUM_CONTROLLER_H
#define AQUARIUM_CONTROLLER_H

//#define useSerial
#include "EEPROM.h"
#define EEPROM_SIZE 32
//Pins
#define TDS_SENSOR_PIN 35
#define TEMP_SENSOR_PIN 13
#define AIR_SRVO_PIN 25//Servo #1
#define LIGHT_SRVO_PIN 27 //Servo #2
#define HEATER_SRVO_PIN 33//Servo #3
#define FILTER_SRVO_PIN 26 //Servo #4
//EEPROM Memory Locations
#define airStateLoc 1
#define lightStateLoc 2
#define heaterStateLoc 3
#define filterStateLoc 4
#define AirOnHrLoc 5
#define AirOnMinLoc 6
#define AirOffHrLoc 7
#define AirOffMinLoc 8
#define Co2OnHrLoc 9
#define Co2OnMinLoc 10
#define Co2OffHrLoc 11
#define Co2OffMinLoc 12
#define LightsOnHrLoc 13
#define LightsOnMinLoc 14
#define LightsOffHrLoc 15
#define LightsOffMinLoc 16
#define aquariumThermostatLoc 17



#endif