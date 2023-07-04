#include "HardwareInterface.h"


HardwareInterface::HardwareInterface() {

}
uint8_t Switch::SwitchStateToInt() {
  switch (this->state) {
    case OFF:
      return 0;
    case ON: 
      return 1;
    case AUXON:
      return 2;
  }
  return 0;
}
SwitchState Switch::IntToSwitchState (uint8_t state) {
  switch (state) {
    case 0:
      return OFF;
    case 1: 
      return ON;
    case 2:
      return AUXON;
  }
  return OFF;
}
void Switch::init(SavedState* savedState) {
  this->savedState = savedState
  this->state = this->IntToSwitchState(savedState->getValue());
  
}
void Switch::setSwitchState(SwitchState state) {
  this->state = state;
  this->savedState->saveValue(this->SwitchStateToInt());
}
void PowerControl::init() {
  this->switch1State = IntToSwitchState(EEPROM.read(airStateLoc));
  this->lightState = IntToSwitchState(EEPROM.read(lightStateLoc));
  this->heaterState = IntToSwitchState(EEPROM.read(heaterStateLoc));
  this->filterState = IntToSwitchState(EEPROM.read(filterStateLoc));
}
void PowerControl::setswitch1State(SwitchState state) {
  this->airState = state;
  EEPROM.write(airStateLoc, SwitchStateToInt(state));
  EEPROM.commit();
}
void PowerControl::setLightState(SwitchState state) {
  this->lightState = state;
  EEPROM.write(lightStateLoc, SwitchStateToInt(state));
  EEPROM.commit();
}
void PowerControl::setHeaterState(SwitchState state) {
  this->heaterState = state;
  EEPROM.write(heaterStateLoc, SwitchStateToInt(state));
  EEPROM.commit();
}
void PowerControl::setFilterState(SwitchState state) {
  this->filterState = state;
  EEPROM.write(filterStateLoc, SwitchStateToInt(state));
  EEPROM.commit();
} 
SwitchState PowerControl::getAirState() {
  return this->airState;
  //return EEPROM.read(airStateLoc);
}
SwitchState PowerControl::getLightState() {
  return this->lightState;
  //return EEPROM.read(lightStateLoc);
}
SwitchState PowerControl::getHeaterState() {
  return this->heaterState;
  //return EEPROM.read(heaterStateLoc);
}
SwitchState PowerControl::getFilterState() {
  return this->filterState;
  //return EEPROM.read(filterStateLoc);
}
SwitchState PowerControl::getDeviceStateByName(String name) {
  if (name == "Lights")
    return this->getLightState();
  if (name == "Air")
    return this->getAirState();
  if (name == "Heater")
    return this->getHeaterState();
  if (name == "Filter")
    return this->getFilterState();
  return OFF;
}
bool PowerControl::airControl(SwitchState powerOption) { //servo 1, switch 1
  //ON POS is 70 (POS1)
  //AUXON POS is 110 (POS2)
  //ON POS = Air Pump on
  //AUXON POS = CO2 on
  #define OFF_OFFSET_1 -8
  #define ON_OFFSET_1 0
  #define AUXON_OFFSET_1 -9
  if (this->airState != powerOption) {
    this->controllerServo.attach(AIR_SRVO_PIN);
    if (powerOption == OFF) {
      this->controllerServo.write(OFF_POS + OFF_OFFSET_1);
      delay(200);
      this->controllerServo.detach();
      this->setAirState(OFF);
      return true;
    }
    if (powerOption == ON) { //Air Pump
    controllerServo.write(POS1 + ON_OFFSET_1);
    delay(200);
    controllerServo.detach();
    this->setAirState(ON);
    return true;
    }
    if (powerOption == AUXON) { //CO2
    controllerServo.write(POS2 + AUXON_OFFSET_1);
    delay(200);
    controllerServo.detach();
    this->setAirState(AUXON);
    return true;
    }
  }
  return false;
}

bool PowerControl::lightControl(SwitchState powerOption) { //servo 2, switch 2
  //ON POS is 110 (POS2) 
  //AUXON POS is 70 (POS1)
  //On POS = Lights on
  //AUXON POS = Not Configured
  #define OFF_OFFSET_2 1
  #define ON_OFFSET_2 -2
  #define AUXON_OFFSET_2 3
  if (this->lightState != powerOption) {
    controllerServo.attach(LIGHT_SRVO_PIN);
    if (powerOption == OFF) { //Lights off
      controllerServo.write(OFF_POS + OFF_OFFSET_2);
      delay(200);
      controllerServo.detach();
      this->setLightState(OFF);
      return true;
    }
    if (powerOption == ON) { //Lights on
      controllerServo.write(POS2 + ON_OFFSET_2);
      delay(200);
      controllerServo.detach();
      this->setLightState(ON);
      return true;
    }
    if (powerOption == AUXON) { //not used
      controllerServo.write(POS1 + AUXON_OFFSET_2);
      delay(200);
      controllerServo.detach();
      this->setLightState(AUXON);
      return true;
    }
  }
  return false;
}
bool PowerControl::heaterControl(SwitchState powerOption) { //servo 3
  //ON POS is 70 (POS1)
  //AUXON POS is 110 (POS2)
  //On POS = Heater on
  //AUXON POS = Not Configured
  #define OFF_OFFSET_3 0
  #define ON_OFFSET_3 0
  #define AUXON_OFFSET_3 -2
  if (this->heaterState != powerOption) {
    controllerServo.attach(HEATER_SRVO_PIN);
    if (powerOption == OFF) { //Heater off
      controllerServo.write(OFF_POS + OFF_OFFSET_3);
      delay(200);
      controllerServo.detach();
      this->setHeaterState(OFF);
      return true;
    }
    if (powerOption == ON) { //Heater on
      controllerServo.write(POS1 + ON_OFFSET_3);
      delay(200);
      controllerServo.detach();
      this->setHeaterState(ON);
      return true;
    }
    if (powerOption == AUXON) { //not used
      controllerServo.write(POS2 + AUXON_OFFSET_3);
      delay(200);
      controllerServo.detach();
      this->setHeaterState(AUXON);
      return true;
    }
  }
  return false;
}
bool PowerControl::filterControl(SwitchState powerOption) { //servo 4
  //ON POS is 110 (POS2) 
  //AUXON POS is 70 (POS1)
  //On POS = Filter on
  //AUXON POS = Not Configured
  #define OFF_OFFSET_4 10
  #define ON_OFFSET_4 10
  #define AUXON_OFFSET_4 10
  if (this->filterState != powerOption) {
    controllerServo.attach(FILTER_SRVO_PIN);
    if (powerOption == OFF) { //Filter off
      controllerServo.write(OFF_POS + OFF_OFFSET_4);
      delay(200);
      controllerServo.detach();
      this->setFilterState(OFF);
      return true;
    }
    if (powerOption == ON) { //Filter on
    controllerServo.write(POS2 + ON_OFFSET_4);
    delay(200);
    controllerServo.detach();
    this->setFilterState(ON);
    return true;
    }
    if (powerOption == AUXON) { //Not used, mapped to X output on switch box
      controllerServo.write(POS1 + AUXON_OFFSET_4);
      delay(200);
      controllerServo.detach();
      this->setFilterState(AUXON);
      return true;
    }
  }
  return false;
}
uint8_t Switch::SwitchStateToInt(SwitchState state) {
  switch (state) {
    case OFF:
      return 0;
    case ON: 
      return 1;
    case AUXON:
      return 2;
  }
  return 0;
}
SwitchState Switch::IntToSwitchState (uint8_t state) {
  switch (state) {
    case 0:
      return OFF;
    case 1: 
      return ON;
    case 2:
      return AUXON;
  }
  return OFF;
}
