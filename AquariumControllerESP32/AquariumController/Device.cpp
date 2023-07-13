#include "Device.h"

Device::Device(String name) {
    this->name = name;
    this->state = OFF;
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
    if (this->state != ON) {
        this->state = ON; 
        if (this->connectedDevice) {
            if(this->connectedDevice->state == ON) {
                this->connectedDevice->state = OFF;
                this->connectedDevice->stateUpdatedByConnectedDevice = true;
            }     
        }
        this->hardwareInterface->powerControl(*this);
    }
    
}
void Device::setStateOff() {
    if (this->state != OFF) {
        this->state = OFF; 
        
        /*if (this->connectedDevice) {
            if(this->connectedDevice->state == ON) {
                this->connectedDevice->state = OFF;
                this->connectedDevice->stateUpdatedByConnectedDevice = true;
            }     
        }*/
        this->hardwareInterface->powerControl(*this);
    }
}