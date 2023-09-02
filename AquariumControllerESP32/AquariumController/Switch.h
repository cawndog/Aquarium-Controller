#ifndef SWITCH_H
#define SWITCH_H
#include <ESP32Servo.h>
#include <Preferences.h>
#include "AquariumController.h"
//--Servo Motor Positions--
#define OFF_POS 90
#define POS1 70
#define POS2 110
enum SwitchState {
  OFF = 0,
  ON = 1,
  AUXON = 2
};
class Switch {
  public: 
    String name;
    SwitchState state;
    Preferences* savedState;
    Servo controllerServo;
    void init(String name, Preferences* savedState);
    void powerControl(SwitchState state) {};
    void setSwitchState(SwitchState state);
    SwitchState getSwitchState();
    uint8_t SwitchStateToInt();
    SwitchState IntToSwitchState(uint8_t state);
};

class Switch1: public Switch {
  public: 
    void powerControl(SwitchState state);
};
class Switch2: public Switch {
  public: 
    void powerControl(SwitchState state);
};
class Switch3: public Switch {
  public: 
    void powerControl(SwitchState state);
};
class Switch4: public Switch {
  public:
    void powerControl(SwitchState state);
};

#endif