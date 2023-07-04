#ifndef DEVICE_H
#define DEVICE_H
//#include <string>
#include "HardwareInterface.h"

enum DeviceState {
    OFF = 0,
    ON = 1
};
class Device {
    public:
        String name;
        DeviceState state;
        Device* connectedDevice;
        bool stateUpdatedByConnectedDevice;
        HardwareInterface* hardwareInterface;

        //function declarations
        Device(String name);
        void init(HardwareInterface* hardwareInterface);
        void attachConnectedDevice(Device* device);
        void setStateOn();
        void setStateOff();
};


#endif