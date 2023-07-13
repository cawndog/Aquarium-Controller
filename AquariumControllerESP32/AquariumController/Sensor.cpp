#include "Sensor.h"

Sensor::Sensor(String name) {
    this->name = name;
    this->value = "0.0";
    this->valueUpdated = false;
}
void Sensor::init(HardwareInterface* hardwareInterface) {
    this->hardwareInterface = hardwareInterface;
}
void TdsSensor::init(HardwareInterface* hardwareInterface, AquariumTemperatureSensor* aqTempSensor) {
    this->hardwareInterface = hardwareInterface;
    this->aqTempSensor = aqTempSensor;
}
String TdsSensor::getValue() {
    return this->value;
}
void TdsSensor::readSensor() {
    this->value = String(this->hardwareInterface->readTdsSensor(this->aqTempSensor->getValue()), 0);
}
void AquariumTemperatureSensor::readSensor() {
    this->value = String(this->hardwareInterface->readAquariumTemperatureSensor(), 1);
}
String AquariumTemperatureSensor::getValue() {
    return this->value;
}


