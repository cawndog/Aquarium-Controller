#ifndef POWERCONTROL_H
#define POWERCONTROL_H


#include <Servo.h>
#include "AquariumController.h"

//--Servo Motor Positions--
#define OFF_POS 90
#define POS1 70
#define POS2 110

#ifdef useSerial
  #include <BluetoothSerial.h>
  extern BluetoothSerial SerialBT;
#endif

enum PowerState {
  OFF = 0,
  ON = 1,
  AUXON = 2,
};
class PowerControl {
  private: 
    Servo controllerServo;
    PowerState heaterState;
    PowerState filterState;
    PowerState airState;
    PowerState lightState;

    void setAirState(PowerState state);
    void setLightState(PowerState state);
    void setHeaterState(PowerState state);
    void setFilterState(PowerState state);

  public:
    PowerControl(); //Constructor
    void init(); //Constructor used in setup() after EEPROM is initialized
    bool filterControl(PowerState powerOption);
    bool airControl(PowerState powerOption);
    bool lightControl(PowerState powerOption);
    bool heaterControl(PowerState powerOption);

    PowerState getAirState();
    PowerState getLightState();
    PowerState getHeaterState();
    PowerState getFilterState();
};
uint8_t PowerStateToInt(PowerState state);
PowerState IntToPowerState (uint8_t state);

#endif

