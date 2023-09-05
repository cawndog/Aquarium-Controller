#ifndef SENSOR_H
#define SENSOR_H
#include <String>
#include "HardwareInterface.h"
class Sensor {
  public:
    typedef std::function<void(Sensor* sensor)> AqWebServerFunction;
    AqWebServerFunction webSocketUpdateState;
    String name;
    String value;
    String prevValue;
    bool valueUpdated;
    HardwareInterface* hardwareInterface;
    //Sensor(String name);
    virtual void init(String name, HardwareInterface* hardwareInterface, AqWebServerFunction webSocketUpdateState);
    virtual String getValue() = 0;
    virtual void readSensor() = 0;
};



class AquariumTemperatureSensor : public Sensor { //Stores value in Fahrenheit
  public:
    AquariumTemperatureSensor();
    void init(String name, HardwareInterface* hardwareInterface, AqWebServerFunction webSocketUpdateState);
    void readSensor();
    String getValue();
};
class TdsSensor : public Sensor {
  public:
    TdsSensor();
    AquariumTemperatureSensor *aqTempSensor;
    void init(String name, HardwareInterface* hardwareInterface, AquariumTemperatureSensor* aqTempSensor, AqWebServerFunction webSocketUpdateState = {});
    void readSensor();
    String getValue();
};
#endif