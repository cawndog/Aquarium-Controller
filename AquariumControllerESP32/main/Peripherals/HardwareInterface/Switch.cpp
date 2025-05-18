#include <ESP32Servo.h>
#include "WString.h"
#include "Switch.h"

// Define servo objects for each channel
Servo servoSwitch1; //Air Servo, Servo #1
Servo servoSwitch2; //Light Servo, Servo #2
Servo servoSwitch3; //Heater Servo, Servo #3
Servo servoSwitch4; //Filter Servo, Servo #4

void Switch::init(String name) {
  if (switchSemaphore == NULL) {
    switchSemaphore = xSemaphoreCreateBinary();
    xSemaphoreGive(switchSemaphore);
  }
  this->name = name;
  String switchStateKey = this->name + "_SW";
  this->state = this->IntToSwitchState(savedState.getUChar(switchStateKey.c_str(), 0));
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
SwitchState Switch::IntToSwitchState(uint8_t state) {
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
void Switch::setSwitchState(SwitchState newState) {
  this->state = newState;
  String switchStateKey = this->name + "_SW";
  savedState.putUChar(switchStateKey.c_str(), this->SwitchStateToInt());
  return;
}
void Switch1::powerControl(SwitchState newState) { //air control switch
  //ON POS is 70 (POS1)
  //AUXON POS is 110 (POS2)
  //ON POS = Air Pump on
  //AUXON POS = CO2 on

  //Motor calibration
  #define OFF_OFFSET_1 -8
  #define ON_OFFSET_1 0
  #define AUXON_OFFSET_1 -9

  if (this->state != newState) {
    xSemaphoreTake(switchSemaphore, portMAX_DELAY);

    servoSwitch1.attach(AIR_SRVO_PIN);
    this->setSwitchState(newState);
    
    if (newState == OFF) {
      servoSwitch1.write(OFF_POS + OFF_OFFSET_1);
    } else if (newState == ON) {
      servoSwitch1.write(POS1 + ON_OFFSET_1);
    } else if (newState == AUXON) {
      servoSwitch1.write(POS2 + AUXON_OFFSET_1);
    }
    vTaskDelay(200 / portTICK_PERIOD_MS);
    
    servoSwitch1.detach();
    xSemaphoreGive(switchSemaphore);
  }
  return;
}

void Switch2::powerControl(SwitchState newState) { // Light control switch
  //ON POS is 110 (POS2) 
  //AUXON POS is 70 (POS1)
  //On POS = Lights on
  //AUXON POS = Not Configured

  //Motor calibration
  #define OFF_OFFSET_2 1
  #define ON_OFFSET_2 -2
  #define AUXON_OFFSET_2 3

  if (this->state != newState) {
    xSemaphoreTake(switchSemaphore, portMAX_DELAY);

    this->setSwitchState(newState);
    servoSwitch2.attach(LIGHT_SRVO_PIN);

    if (newState == OFF) {
      servoSwitch2.write(OFF_POS + OFF_OFFSET_2);
    } else if (newState == ON) {
      servoSwitch2.write(POS2 + ON_OFFSET_2);
    }
    // Uncomment if AUXON is needed
    // else if (newState == AUXON) {
    //   servoSwitch2.write(POS1 + AUXON_OFFSET_2);
    // }
    vTaskDelay(200 / portTICK_PERIOD_MS);

    servoSwitch2.detach();
    xSemaphoreGive(switchSemaphore);
  }
  return;
}

void Switch3::powerControl(SwitchState newState) { // Heater control switch
  //ON POS is 70 (POS1)
  //AUXON POS is 110 (POS2)
  //On POS = Heater on
  //AUXON POS = Not Configured

  //Motor calibration
  #define OFF_OFFSET_3 0
  #define ON_OFFSET_3 0
  #define AUXON_OFFSET_3 -2

  if (this->state != newState) {
    xSemaphoreTake(switchSemaphore, portMAX_DELAY);

    this->setSwitchState(newState);
    servoSwitch3.attach(HEATER_SRVO_PIN);

    if (newState == OFF) {
      servoSwitch3.write(OFF_POS + OFF_OFFSET_3);
    } else if (newState == ON) {
      servoSwitch3.write(POS1 + ON_OFFSET_3);
    }
    // Uncomment if AUXON is needed
    //else if (newState == AUXON) {
    //  servoSwitch3.write(POS2 + AUXON_OFFSET_3);
    //}
    vTaskDelay(200 / portTICK_PERIOD_MS);

    servoSwitch3.detach();
    xSemaphoreGive(switchSemaphore);
  }
  return;
}

void Switch4::powerControl(SwitchState newState) { // Filter control switch
  //ON POS is 110 (POS2) 
  //AUXON POS is 70 (POS1)
  //On POS = Filter on
  //AUXON POS = Not Configured

  //Motor calibration
  #define OFF_OFFSET_4 10
  #define ON_OFFSET_4 10
  #define AUXON_OFFSET_4 10

  if (this->state != newState) {
    xSemaphoreTake(switchSemaphore, portMAX_DELAY);

    this->setSwitchState(newState);
    servoSwitch4.attach(FILTER_SRVO_PIN);

    if (newState == OFF) {
      servoSwitch4.write(OFF_POS + OFF_OFFSET_4);
    } else if (newState == ON) {
      servoSwitch4.write(POS2 + ON_OFFSET_4);
    }
    // Uncomment if AUXON is needed
    //else if (newState == AUXON) {
    //  servoSwitch4.write(POS1 + AUXON_OFFSET_4);
    //}
    vTaskDelay(200 / portTICK_PERIOD_MS);

    servoSwitch4.detach();
    xSemaphoreGive(switchSemaphore);
  }
  return;
}