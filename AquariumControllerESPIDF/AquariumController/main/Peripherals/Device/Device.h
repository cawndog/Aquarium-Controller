
#ifndef DEVICE_H
#define DEVICE_H
#include <Arduino.h>
#include "HardwareInterface.h"
//class HardwareInterface;

enum DeviceState {
  DEVICE_OFF,
  DEVICE_ON
};
class Device {
  public:
    typedef std::function<bool(bool newState)> OkToExecute;
    typedef std::function<void(Device** devices, int numDevices)> PostExecutionFunction;
    OkToExecute okToExe;
    PostExecutionFunction postExeFunction;
    String name;
    DeviceState state;
    Device* connectedDevice;
    HardwareInterface* hardwareInterface;

    //function declarations
    Device();
    void init(String name, HardwareInterface* hardwareInterface, OkToExecute = [](bool newState) {return true;}, PostExecutionFunction = [](Device** devices, int numDevices){});
    void attachConnectedDevice(Device* device);
    void setStateOn(bool overrideOkToExe = false);
    void setStateOff(bool overrideOkToExe = false);
    bool getStateBool();
    static uint8_t deviceStateToInt(DeviceState state);
    static DeviceState intToDeviceState(uint8_t stateInt);
};


#endif