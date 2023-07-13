#ifndef HARDWAREINTERFACE_H
#define HARDWAREINTERFACE_H


#include <Servo.h>
#include "../AquariumController.h"
#include "../Device.h"
#include "../SavedStateController.h"
#include "Switch.h"
#include <OneWire.h>
#include <DallasTemperature.h>

//--Servo Motor Positions--
#define OFF_POS 90
#define POS1 70
#define POS2 110

#ifdef useSerial
  #include <BluetoothSerial.h>
  extern BluetoothSerial SerialBT;
#endif


class HardwareInterface {
  private: 
    Switch1 switch1; //air switch
    Switch2 switch2; //light switch
    Switch3 switch3; //heater switch
    Switch4 switch4; //filter switch

    OneWire *oneWire;
    DallasTemperature *tempSensors;
    int getMedianNum(int bArray[], int iFilterLen); //For reading TDS value

  public:
    HardwareInterface(); //Constructor
    void init(SavedStateController* savedStateController); //Constructor used in setup() after EEPROM is initialized
    DeviceState initDeviceState(Device* device);
    void powerControl(Device* device); //power control interface for a device. Turns a device's corresponding switch on/off depending on the state of the device. 
    float readTdsSensor(float temperature);
    float readAquariumTemperatureSensor();

};


#endif

