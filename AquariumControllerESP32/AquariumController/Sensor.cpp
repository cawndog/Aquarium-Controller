#include "Sensor.h"
/*Sensor::Sensor(String name) {
  this->name = name;
  this->value = "0.0";
  this->valueUpdated = false;
}*/
TdsSensor::TdsSensor() {
    webSocketUpdateState = {};
    this->value = "0.0";
    this->prevValue = "0.0";
    this->valueUpdated = false;
}
AquariumTemperatureSensor::AquariumTemperatureSensor() {
    webSocketUpdateState = {};
    this->value = "0.0";
    this->prevValue = "0.0";
    this->valueUpdated = false;
}
void Sensor::init(String name, HardwareInterface* hardwareInterface, AqWebServerFunction webSocketUpdateState) {
    this->name = name;
    this->hardwareInterface = hardwareInterface;
}
void TdsSensor::init(String name, HardwareInterface* hardwareInterface, AquariumTemperatureSensor* aqTempSensor, AqWebServerFunction webSocketUpdateState) {
    this->name = name;
    this->hardwareInterface = hardwareInterface;
    this->aqTempSensor = aqTempSensor;
}
String TdsSensor::getValue() {
    return this->value;
}
void TdsSensor::readSensor() {
    prevValue = this->value;
    this->value = String(this->hardwareInterface->readTdsSensor(this->aqTempSensor->getValue().toFloat()), 0);
    this->webSocketUpdateState(this);
}
void AquariumTemperatureSensor::readSensor() {
    prevValue = this->value;
    this->value = String(this->hardwareInterface->readAquariumTemperatureSensor(), 1);
    this->webSocketUpdateState(this);
}
String AquariumTemperatureSensor::getValue() {
    return this->value;
}

