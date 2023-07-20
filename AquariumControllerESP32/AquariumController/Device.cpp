#include "Device.h"

Device::Device(String name) {
    this->name = name;
    this->state = DEVICE_OFF;
    this->connectedDevice = NULL;
    this->stateUpdatedByConnectedDevice = false;
    this->hardwareInterface = NULL;
}
void Device::init(HardwareInterface* hardwareInterface) {
    this->hardwareInterface = hardwareInterface;
    this->state = hardwareInterface->initDeviceState(*this);
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
        this->hardwareInterface->powerControl(*this);
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
        this->hardwareInterface->powerControl(*this);
    }
}