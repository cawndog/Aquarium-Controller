#ifndef SENSOR_H
#define SENSOR_H
#include <String>
#include "HardwareInterface.h"
class Sensor {
  public:
    String name;
    String value;
    bool valueUpdated;
    HardwareInterface* hardwareInterface;
    //Sensor(String name);
    void init(String name, HardwareInterface* hardwareInterface);
    virtual String getValue() = 0;
    virtual void readSensor() = 0;
};



class AquariumTemperatureSensor : public Sensor { //Stores value in Fahrenheit
  public:
    AquariumTemperatureSensor();
    void readSensor();
    String getValue();
};
class TdsSensor : public Sensor {
  public:
    TdsSensor();
    AquariumTemperatureSensor *aqTempSensor;
    void init(String name, HardwareInterface* hardwareInterface, AquariumTemperatureSensor* aqTempSensor);
    void readSensor();
    String getValue();
};
#endif