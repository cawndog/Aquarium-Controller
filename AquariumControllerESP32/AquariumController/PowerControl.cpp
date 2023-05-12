#include "PowerControl.h"

PowerControl::PowerControl() {
  this->airState = OFF;
  this->lightState = OFF;
  this->heaterState = OFF;
  this->filterState = OFF;
}
void PowerControl::init() {
  this->airState = IntToPowerState(EEPROM.read(airStateLoc));
  this->lightState = IntToPowerState(EEPROM.read(lightStateLoc));
  this->heaterState = IntToPowerState(EEPROM.read(heaterStateLoc));
  this->filterState = IntToPowerState(EEPROM.read(filterStateLoc));
}
void PowerControl::setAirState(PowerState state) {
  this->airState = state;
  EEPROM.write(airStateLoc, PowerStateToInt(state));
  EEPROM.commit();
}
void PowerControl::setLightState(PowerState state) {
  this->lightState = state;
  EEPROM.write(lightStateLoc, PowerStateToInt(state));
  EEPROM.commit();
}
void PowerControl::setHeaterState(PowerState state) {
  this->heaterState = state;
  EEPROM.write(heaterStateLoc, PowerStateToInt(state));
  EEPROM.commit();
}
void PowerControl::setFilterState(PowerState state) {
  this->filterState = state;
  EEPROM.write(filterStateLoc, PowerStateToInt(state));
  EEPROM.commit();
} 
PowerState PowerControl::getAirState() {
  return this->airState;
  //return EEPROM.read(airStateLoc);
}
PowerState PowerControl::getLightState() {
  return this->lightState;
  //return EEPROM.read(lightStateLoc);
}
PowerState PowerControl::getHeaterState() {
  return this->heaterState;
  //return EEPROM.read(heaterStateLoc);
}
PowerState PowerControl::getFilterState() {
  return this->filterState;
  //return EEPROM.read(filterStateLoc);
}
bool PowerControl::airControl(PowerState powerOption) { //servo 1
  //ON POS is 70 (POS1)
  //AUXON POS is 110 (POS2)
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

bool PowerControl::lightControl(PowerState powerOption) { //servo 2
  //ON POS is 110 (POS2) 
  //AUXON POS is 70 (POS1)
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
bool PowerControl::heaterControl(PowerState powerOption) { //servo 3
  //ON POS is 70 (POS1)
  //AUXON POS is 110 (POS2)
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
bool PowerControl::filterControl(PowerState powerOption) { //servo 4
  //ON POS is 110 (POS2) 
  //AUXON POS is 70 (POS1)
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
uint8_t PowerStateToInt(PowerState state) {
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
PowerState IntToPowerState (uint8_t state) {
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
