#include "Device.h"

Device::Device() {
    this->name = "";
    this->state = DEVICE_OFF;
    this->connectedDevice = NULL;
    this->hardwareInterface = NULL;
    webSocketUpdateState = [](Device** devices, int numDevices){};
}
void Device::init(String name, HardwareInterface* hardwareInterface, AqWebServerDevFunction webSocketUpdateState) {
    this->name = name;
    this->hardwareInterface = hardwareInterface;
    this->state = intToDeviceState(hardwareInterface->initDeviceState(this->name));
    this->webSocketUpdateState = webSocketUpdateState;
}
void Device::attachConnectedDevice(Device* device) {
    this->connectedDevice = device;
}
void Device::setStateOn() {
    if (this->state != DEVICE_ON) {
        this->state = DEVICE_ON; 
        Device* devicesUpdated[2];
        devicesUpdated[0] = this;
        if (this->connectedDevice != NULL) {
            if(this->connectedDevice->state == DEVICE_ON) {
                this->connectedDevice->state = DEVICE_OFF;
                devicesUpdated[1] = this->connectedDevice;
                this->webSocketUpdateState(devicesUpdated, 2);
            }
            else {
                this->webSocketUpdateState(devicesUpdated, 1);
            }     
        }else {
            this->webSocketUpdateState(devicesUpdated, 1);
        }
        this->hardwareInterface->powerControl(this->name, deviceStateToInt(this->state));
    }
    
}
void Device::setStateOff() {
    if (this->state != DEVICE_OFF) {
        this->state = DEVICE_OFF;
        Device* devicesUpdated[2];
        devicesUpdated[0] = this; 
        this->webSocketUpdateState(devicesUpdated, 1);
        /*if (this->connectedDevice) {
            if(this->connectedDevice->state == DEVICE_ON) {
                this->connectedDevice->state = DEVICE_OFF;
                this->connectedDevice->stateUpdatedByConnectedDevice = true;
            }     
        }*/
        this->hardwareInterface->powerControl(this->name, deviceStateToInt(this->state));
    }
}
bool Device::getStateBool() {
    if (this->state == DEVICE_ON) {
        return true;
    }
    else {
        return false; 
    }
}
uint8_t Device::deviceStateToInt(DeviceState state) {
    switch(state) {
        case DEVICE_OFF: {
            return 0;
        }
        case DEVICE_ON: {
            return 1;
        }
        default: 
            return 0;
    }
}
DeviceState Device::intToDeviceState(uint8_t stateInt) {
    switch(stateInt) {
        case 0:
            return DEVICE_OFF;
        case 1: 
            return DEVICE_ON;
        default: 
            return DEVICE_OFF;
    }

}