#ifndef SWITCH_H
#define SWITCH_H

#include <ESP32Servo.h>
#include <Preferences.h>
#include "AquariumController.h"
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <Arduino.h>

// Servo positions
#define OFF_POS 90
#define POS1    70
#define POS2    110

extern Preferences savedState;
//extern volatile SemaphoreHandle_t switchSemaphore;
static volatile SemaphoreHandle_t switchSemaphore = NULL;

// Switch states
enum SwitchState {
  OFF = 0,
  ON = 1,
  AUXON = 2
};

class Switch {
  public:
    String name;
    SwitchState state;
    void init(String name);
    void setSwitchState(SwitchState newState);
    SwitchState getSwitchState();
    uint8_t SwitchStateToInt();
    SwitchState IntToSwitchState(uint8_t state);
    virtual void powerControl(SwitchState newState) {};
};

class Switch1 : public Switch {
  public:
    void powerControl(SwitchState newState) override;
};
class Switch2 : public Switch {
  public:
    void powerControl(SwitchState newState) override;
};
class Switch3 : public Switch {
  public:
    void powerControl(SwitchState newState) override;
};
class Switch4 : public Switch {
  public:
    void powerControl(SwitchState newState) override;
};

#endif