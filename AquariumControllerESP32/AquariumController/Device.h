
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
        typedef std::function<void(Device* device)> AqWebServerFunction;
        AqWebServerFunction webSocketUpdateState;
        String name;
        DeviceState state;
        Device* connectedDevice;
        HardwareInterface* hardwareInterface;

        //function declarations
        Device();
        void init(String name, HardwareInterface* hardwareInterface, AqWebServerFunction webSocketUpdateState = {});
        void attachConnectedDevice(Device* device);
        void setStateOn();
        void setStateOff();
        bool getStateBool();
        static uint8_t deviceStateToInt(DeviceState state);
        static DeviceState intToDeviceState(uint8_t stateInt);
};


#endif