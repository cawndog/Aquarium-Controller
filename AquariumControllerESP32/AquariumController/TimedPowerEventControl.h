#ifndef TIMED_POWER_EVENT_CONTROL_H
#define TIMED_POWER_EVENT_CONTROL_H

#include "AquariumController.h"
#include "PowerControl.h"
#include <ESP32Time.h>
#ifdef useSerial
  #include <BluetoothSerial.h>
  extern BluetoothSerial SerialBT;
#endif
class TimedDevice {
  public:
    String deviceName;
    uint8_t onHr;
    uint8_t onMin;
    uint8_t offHr;
    uint8_t offMin;
    uint8_t onHrSaveLoc;
    uint8_t onMinSaveLoc;
    uint8_t offHrSaveLoc;
    uint8_t offMinSaveLoc;
    unsigned long nextEventEpochTime; 
    TimedDevice();
    void init(String deviceName, uint8_t onHrLoc, uint8_t onMinLoc, uint8_t offHrLoc, int8_t offMinLoc, ESP32Time *rtc, PowerControl *powerControl);
    void updateDeviceState(ESP32Time *rtc, PowerControl *powerControl);
    void initDeviceState(ESP32Time *rtc, PowerControl *powerControl);  
    uint8_t getOnHr();
    uint8_t getOnMin();
    uint8_t getOffHr();
    uint8_t getOffMin();
    void setOnHr(uint8_t setVal);
    void setOnMin(uint8_t setVal);
    void setOffHr(uint8_t setVal);
    void setOffMin(uint8_t setVal);

  private:
    bool turnDeviceOn(PowerControl *powerControl);
    bool turnDeviceOff(PowerControl *powerControl);
}; 
class TimedPowerEventControl {
  private:
    uint8_t numDevices;
    PowerControl *powerControl;
    ESP32Time *rtc;
  public:
  TimedDevice devices[6];
    TimedDevice *nextDeviceWithEvent;
    TimedPowerEventControl();
    void init(ESP32Time *rtc, PowerControl *powerControl);
    void initNewSettings();
    uint8_t getNumDevices();
    void determineDeviceWithNextEvent(); //determines which device is the next one that will have a timed event and stores it in nextDeviceWithEvent. 
    void handleEventTrigger(); //handles the timed power event. Then calls determineNextEventDevice()
    TimedDevice* getDeviceByName(String name);
    TimedDevice* getDeviceByNumber(uint8_t num);
};
#endif