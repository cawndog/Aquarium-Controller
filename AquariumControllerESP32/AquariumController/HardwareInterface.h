#ifndef HARDWAREINTERFACE_H
#define HARDWAREINTERFACE_H


#include <Servo.h>
#include "AquariumController.h"
#include "Device.h"

//--Servo Motor Positions--
#define OFF_POS 90
#define POS1 70
#define POS2 110

#ifdef useSerial
  #include <BluetoothSerial.h>
  extern BluetoothSerial SerialBT;
#endif

class Switch {
  enum SwitchState {
    OFF = 0,
    ON = 1,
    AUXON = 2
  }
  SwitchState state;
  SavedState* savedState;
  init(SavedState* savedState);
  void setSwitchState(SwitchState state);
  uint8_t SwitchStateToInt();
  SwitchState IntToSwitchState(uint8_t state);
};
class HardwareInterface {
  private: 
    Servo controllerServo;
    Switch switch1; //air switch
    Switch switch2; //light switch
    Switch switch3; //heater switch
    Switch switch4; //filter switch


    void setAirState(SwitchState state);
    void setLightState(SwitchState state);
    void setHeaterState(SwitchState state);
    void setFilterState(SwitchState state);

  public:
    HardwareInterface(); //Constructor
    void init(); //Constructor used in setup() after EEPROM is initialized
    bool filterControl(SwitchState powerOption);
    bool airControl(SwitchState powerOption);
    bool lightControl(SwitchState powerOption);
    bool heaterControl(SwitchState powerOption);

    SwitchState getAirState();
    SwitchState getLightState();
    SwitchState getHeaterState();
    SwitchState getFilterState();
    SwitchState getDeviceStateByName(String name);
};
uint8_t SwitchStateToInt(SwitchState state);
SwitchState IntToSwitchState (uint8_t state);

#endif

