#include "Alarm.h"

Alarm::Alarm() {
  this->name = "";
  this->alarmState = 0;
  this->setAlarmStateFunction = NULL;
}
void Alarm::init(String name, String shortName, SetAlarmStateFunction setAlarmStateFunction) {
  this->name = name;
  this->shortName = shortName;
  String alarmKey= this->shortName + "_K";
  this->alarmState = savedState.getChar(alarmKey.c_str(), 0);
  this->setAlarmStateFunction = setAlarmStateFunction;
}
String Alarm::getName() {
  return this->name;
}
int8_t Alarm::getAlarmState() {
  return this->alarmState;
}
void Alarm::setAlarmState(int8_t newState) {
  this->alarmState = newState;
  String alarmKey= this->shortName + "_K";
  savedState.putChar(alarmKey.c_str(), newState);
  this->setAlarmStateFunction(this);
}