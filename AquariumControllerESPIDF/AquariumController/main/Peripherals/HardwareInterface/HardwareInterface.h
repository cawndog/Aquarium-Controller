#ifndef HARDWAREINTERFACE_H
#define HARDWAREINTERFACE_H

#include "AquariumController.h"

#include "Switch.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Preferences.h>
#include <Arduino.h>
#define VREF 3.3              // analog reference voltage(Volt) of the ADC
#define SCOUNT  30            // sum of sample point
#ifdef useSerialBT
  #include <BleSerial.h>
  extern BleSerial SerialBT;
#endif

class Sensor;
static volatile SemaphoreHandle_t waterValveSemaphore;

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
    ~HardwareInterface();
    void init(); //Constructor used in setup() after EEPROM is initialized
    uint8_t initDeviceState(String deviceName);
    void powerControl(String deviceName, uint8_t state); //power control interface for a device. Turns a device's corresponding switch on/off depending on the state of the device. 
    float readTdsSensor(float temperature);
    float readAquariumTemperatureSensor();

};


#endif

