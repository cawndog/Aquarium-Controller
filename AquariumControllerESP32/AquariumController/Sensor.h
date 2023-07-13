#ifndef SENSOR_H
#define SENSOR_H
#include <String>
#include "HardwareInterface/HardwareInterface.h"
class Sensor {
  public:
    String name;
    String value;
    bool valueUpdated;
 
    Sensor(String name);
    void init(HardwareInterface* hardwareInterface);
    virtual String getValue() = 0;
    virtual void readSensor() = 0;
}

class TdsSensor : public Sensor {
  public:
    AquariumTemperatureSensor *aqTempSensor;
    void init(HardwareInterface* hardwareInterface, AquariumTemperatureSensor* aqTempSensor);
    void readSensor();
}

class AquariumTemperatureSensor : public Sensor { //Stores value in Fahrenheit
  public:
    void readSensor();

}
#endif