#include "Device.h"

Device::Device() {
    this->state = DEVICE_OFF;
    this->connectedDevice = NULL;
    this->stateUpdatedByConnectedDevice = false;
    this->hardwareInterface = NULL;
}
void Device::init(String name, HardwareInterface* hardwareInterface) {
    this->name = name;
    this->hardwareInterface = hardwareInterface;
    this->state = intToDeviceState(hardwareInterface->initDeviceState(this->name));
}
void Device::attachConnectedDevice(Device* device) {
    this->connectedDevice = device;
}
void Device::setStateOn() {
    if (this->state != DEVICE_ON) {
        this->state = DEVICE_ON; 
        if (this->connectedDevice) {
            if(this->connectedDevice->state == DEVICE_ON) {
                this->connectedDevice->state = DEVICE_OFF;
                this->connectedDevice->stateUpdatedByConnectedDevice = true;
            }     
        }
        this->hardwareInterface->powerControl(this->name, deviceStateToInt(this->state));
    }
    
}
void Device::setStateOff() {
    if (this->state != DEVICE_OFF) {
        this->state = DEVICE_OFF; 
        
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