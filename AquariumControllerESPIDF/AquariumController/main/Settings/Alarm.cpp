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
  String overrideKey = this->shortName + "O_K";
  this->alarmState = savedState.getULong(alarmKey.c_str(), 0);
  this->alarmOverride = savedState.getBool(overrideKey.c_str(), false);
  this->setAlarmStateFunction = setAlarmStateFunction;
}
String Alarm::getName() {
  return this->name;
}
unsigned long Alarm::getAlarmState() {
  return this->alarmState;
}
bool Alarm::getAlarmOverride() {
  return this->alarmOverride;
}
void Alarm::setAlarmState(unsigned long newState) {
  this->alarmState = newState;
  String alarmKey= this->shortName + "_K";
  savedState.putULong(alarmKey.c_str(), newState);
  this->setAlarmStateFunction(this, false);
}
void Alarm::setAlarmOverride(bool override) {
  this->alarmOverride = override;
  String overrideKey = this->shortName + "O_K";
  savedState.putBool(overrideKey.c_str(), override);
  this->setAlarmStateFunction(this, true);
}