#include "Switch.h"

void Switch::init(String name, Preferences* savedState) {
  this->name = name;
  this->savedState = savedState;
  const char* namePtr = &(this->name[0]);
  this->state = this->IntToSwitchState(savedState->getUChar(namePtr, 0));
  
}
SwitchState Switch::getSwitchState() {
  return this->state;
}
uint8_t Switch::SwitchStateToInt() {
  switch (this->state) {
    case OFF:
      return 0;
    case ON: 
      return 1;
    case AUXON:
      return 2;
  };
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
    default: 
      return OFF;
  };
}
void Switch::setSwitchState(SwitchState state) {
  this->state = state;
  const char* namePtr = &(this->name[0]);
  this->savedState->putUChar(namePtr, this->SwitchStateToInt());
  return;
}
void Switch1::powerControl(SwitchState state) { //air control switch
  //ON POS is 70 (POS1)
  //AUXON POS is 110 (POS2)
  //ON POS = Air Pump on
  //AUXON POS = CO2 on
  #define OFF_OFFSET_1 -8
  #define ON_OFFSET_1 0
  #define AUXON_OFFSET_1 -9

  if (this->state != state) {
    this->setSwitchState(state);
    this->controllerServo.attach(AIR_SRVO_PIN);
    if (state == OFF) {
      this->controllerServo.write(OFF_POS + OFF_OFFSET_1);
      delay(200);
      this->controllerServo.detach();
      return;
    }
    if (state == ON) { //Air Pump
      this->controllerServo.write(POS1 + ON_OFFSET_1);
      delay(200);
      this->controllerServo.detach();
      return;
    }
    if (state == AUXON) { //CO2
      this->controllerServo.write(POS2 + AUXON_OFFSET_1);
      delay(200);
      this->controllerServo.detach();
      return;
    }
  }
  return;
}
void Switch2::powerControl(SwitchState state) { //light control switch
  //ON POS is 110 (POS2) 
  //AUXON POS is 70 (POS1)
  //On POS = Lights on
  //AUXON POS = Not Configured
  #define OFF_OFFSET_2 1
  #define ON_OFFSET_2 -2
  #define AUXON_OFFSET_2 3
  this->setSwitchState(state);
  this->controllerServo.attach(LIGHT_SRVO_PIN);
  if (state == OFF) {
    this->controllerServo.write(OFF_POS + OFF_OFFSET_2);
    delay(200);
    this->controllerServo.detach();
    return;
  }
  if (state == ON) { 
    this->controllerServo.write(POS2 + ON_OFFSET_2);
    delay(200);
    this->controllerServo.detach();
    return;
  }
  /*if (state == AUXON) { 
    this->controllerServo.write(POS2 + AUXON_OFFSET_2);
    delay(200);
    this->controllerServo.detach();
    return;
  }*/
  return;
}
void Switch3::powerControl(SwitchState state) { //heater control switch
  //ON POS is 70 (POS1)
  //AUXON POS is 110 (POS2)
  //On POS = Heater on
  //AUXON POS = Not Configured
  #define OFF_OFFSET_3 0
  #define ON_OFFSET_3 0
  #define AUXON_OFFSET_3 -2
  this->setSwitchState(state);
  this->controllerServo.attach(AIR_SRVO_PIN);
  if (state == OFF) {
    this->controllerServo.write(OFF_POS + OFF_OFFSET_3);
    delay(200);
    this->controllerServo.detach();
    return;
  }
  if (state == ON) { 
    this->controllerServo.write(POS1 + ON_OFFSET_3);
    delay(200);
    this->controllerServo.detach();
    return;
  }
  /*if (state == AUXON) { 
    this->controllerServo.write(POS2 + AUXON_OFFSET_3);
    delay(200);
    this->controllerServo.detach();
    return;
  }*/
  return;
}
void Switch4::powerControl(SwitchState state) { //filter control switch
  //ON POS is 110 (POS2) 
  //AUXON POS is 70 (POS1)
  //On POS = Filter on
  //AUXON POS = Not Configured
  #define OFF_OFFSET_4 10
  #define ON_OFFSET_4 10
  #define AUXON_OFFSET_4 10
  this->setSwitchState(state);
  this->controllerServo.attach(FILTER_SRVO_PIN);
  if (state == OFF) {
    this->controllerServo.write(OFF_POS + OFF_OFFSET_4);
    delay(200);
    this->controllerServo.detach();
    return;
  }
  if (state == ON) {
    this->controllerServo.write(POS2 + ON_OFFSET_4);
    delay(200);
    this->controllerServo.detach();
    return;
  }
  /*if (state == AUXON) { 
    this->controllerServo.write(POS2 + AUXON_OFFSET_4);
    delay(200);
    this->controllerServo.detach();
    return;
  }*/
  return;
}