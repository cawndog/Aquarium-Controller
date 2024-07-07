#include "Setting.h"

GeneralSetting::GeneralSetting() {
  this->name = "";
  this->value = 0;
  this->setValueFunction = NULL;
}
void GeneralSetting::init(String name, String shortName, SetValueFunction setValueFunction) {
  this->name = name;
  this->shortName = shortName;
  String settingKey= this->shortName + "_K";
  this->value = savedState.getChar(settingKey.c_str(), 0);
  this->setValueFunction = setValueFunction;
}
String GeneralSetting::getName() {
  return this->name;
}
int8_t GeneralSetting::getValue() {
  return this->value;
}
void GeneralSetting::setValue(int8_t newVal) {
  this->value = newVal;
  String settingKey= this->shortName + "_K";
  savedState.putChar(settingKey.c_str(), newVal);
  this->setValueFunction(this);
}