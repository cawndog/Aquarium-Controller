#ifndef SWITCH_H
#define SWITCH_H
#include <ESP32Servo.h>
#include <Preferences.h>
#include "AquariumController.h"
//--Servo Motor Positions--
#define OFF_POS 90
#define POS1 70
#define POS2 110
static volatile SemaphoreHandle_t switchSemaphore = NULL;
extern Preferences savedState;
enum SwitchState {
  OFF = 0,
  ON = 1,
  AUXON = 2
};
class Switch {
  public: 
    String name;
    SwitchState state;
    Servo controllerServo;
    void init(String name);
    void powerControl(SwitchState newState) {};
    void setSwitchState(SwitchState newState);
    SwitchState getSwitchState();
    uint8_t SwitchStateToInt();
    SwitchState IntToSwitchState(uint8_t state);
};

class Switch1: public Switch {
  public: 
    void powerControl(SwitchState newState);
};
class Switch2: public Switch {
  public: 
    void powerControl(SwitchState newState);
};
class Switch3: public Switch {
  public: 
    void powerControl(SwitchState newState);
};
class Switch4: public Switch {
  public:
    void powerControl(SwitchState newState);
};

#endif