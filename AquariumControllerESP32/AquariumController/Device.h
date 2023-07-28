
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
        String name;
        DeviceState state;
        Device* connectedDevice;
        bool stateUpdatedByConnectedDevice;
        HardwareInterface* hardwareInterface;

        //function declarations
        Device();
        void init(String name, HardwareInterface* hardwareInterface);
        void attachConnectedDevice(Device* device);
        void setStateOn();
        void setStateOff();
        static uint8_t deviceStateToInt(DeviceState state);
        static DeviceState intToDeviceState(uint8_t stateInt);
};


#endif