#ifndef SWITCH_H
#define SWITCH_H
#include <Servo.h>
#include "../SavedStateController.h"
enum SwitchState {
  OFF = 0,
  ON = 1,
  AUXON = 2
};
class Switch {

  SwitchState state;
  SavedState* savedState;
  Servo controllerServo;
  init(SavedState* savedState);
  void powerControl(SwitchState state);
  void setSwitchState(SwitchState state);
  SwitchState getSwitchState();
  uint8_t SwitchStateToInt();
  SwitchState IntToSwitchState(uint8_t state);
};

class Switch1: public Switch {
  void powerControl(SwitchState state);
};
class Switch2: public Switch {
  void powerControl(SwitchState state);
};
class Switch3: public Switch {
  void powerControl(SwitchState state);
};
class Switch4: public Switch {
  void powerControl(SwitchState state);
};

#endif