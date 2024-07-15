#include "Sensor.h"
/*Sensor::Sensor(String name) {
  this->name = name;
  this->value = "0.0";
  this->valueUpdated = false;
}*/
TdsSensor::TdsSensor() {
  webSocketUpdateState = [](Sensor* sensor){};
  //webSocketUpdateState = {};
  this->value = "0.0";
  this->prevValue = "0.0";
  this->valueUpdated = false;
}
AquariumTemperatureSensor::AquariumTemperatureSensor() {
  webSocketUpdateState = [](Sensor* sensor){};
  //webSocketUpdateState = {};
  this->value = "0.0";
  this->prevValue = "0.0";
  this->valueUpdated = false;
}
WaterSensor::WaterSensor() {
  webSocketUpdateState = NULL;
  //webSocketUpdateState = {};
  this->value = 0;
  //this->prevValue = 0;
  this->valueUpdated = false;
}
void Sensor::init(String name, HardwareInterface* hardwareInterface, AqWebServerFunction webSocketUpdateState) {
  this->name = name;
  this->hardwareInterface = hardwareInterface;
  this->webSocketUpdateState = webSocketUpdateState;
}
void TdsSensor::init(String name, HardwareInterface* hardwareInterface, AquariumTemperatureSensor* aqTempSensor, AqWebServerFunction webSocketUpdateState) {
  this->name = name;
  this->hardwareInterface = hardwareInterface;
  this->aqTempSensor = aqTempSensor;
  this->webSocketUpdateState = webSocketUpdateState;
}
void AquariumTemperatureSensor::init(String name, HardwareInterface* hardwareInterface, AqWebServerFunction webSocketUpdateState) {
  this->name = name;
  this->hardwareInterface = hardwareInterface;
  this->webSocketUpdateState = webSocketUpdateState;
}
void WaterSensor::init(String name) {
  this->name = name;
}
String TdsSensor::getValue() {
return this->value;
}
String AquariumTemperatureSensor::getValue() {
  return this->value;
}
String WaterSensor::getValue() {
  return (String)this->value;
}
int WaterSensor::getValueInt() {
  return this->value;
}
void TdsSensor::readSensor() {
  prevValue = this->value;
  this->value = String(this->hardwareInterface->readTdsSensor(this->aqTempSensor->getValue().toFloat()), 0);
  this->webSocketUpdateState(this);
}
void AquariumTemperatureSensor::readSensor() {
  prevValue = this->value;
  float fval = this->hardwareInterface->readAquariumTemperatureSensor();
  Serial.printf("AqTemp as float: %f\n", fval);
  this->value = String(fval, 1);
  Serial.printf("AqTemp as String: %s\n", this->value.c_str());
  this->webSocketUpdateState(this);
}
void WaterSensor::readSensor() {
  this->value = analogRead(WATER_SENSOR_PIN);
}



