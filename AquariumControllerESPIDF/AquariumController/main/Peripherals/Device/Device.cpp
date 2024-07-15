#include "Device.h"

Device::Device() {
  this->name = "";
  this->state = DEVICE_OFF;
  this->connectedDevice = NULL;
  this->hardwareInterface = NULL;
  okToExe = [](bool newState){return true;};
  postExeFunction = [](Device** devices, int numDevices){};
}

void Device::init(String name, HardwareInterface* hardwareInterface, OkToExecute okToExe, PostExecutionFunction postExeFunction) {
  this->name = name;
  this->hardwareInterface = hardwareInterface;
  this->state = intToDeviceState(hardwareInterface->initDeviceState(this->name));
  this->postExeFunction = postExeFunction;
}
void Device::attachConnectedDevice(Device* device) {
  this->connectedDevice = device;
}
void Device::setStateOn(bool overrideOkToExe) {
  bool willExe = (overrideOkToExe == true) ? true : this->okToExe(true);
  if (willExe) {
  //if (this->okToExe(true) == true) {
    if (this->state != DEVICE_ON) {
      this->state = DEVICE_ON; 
      TaskHandle_t xHandle = NULL;
      xTaskCreate([](void* pvParameters) {
        Device* device = (Device*) pvParameters;
        Device* devicesUpdated[2];
        devicesUpdated[0] = device;
        if (device->connectedDevice != NULL) {
          if(device->connectedDevice->state == DEVICE_ON) {
            device->connectedDevice->state = DEVICE_OFF;
            devicesUpdated[1] = device->connectedDevice;
            device->postExeFunction(devicesUpdated, 2);
          }
          else {
            device->postExeFunction(devicesUpdated, 1);
          }     
        }else {
          device->postExeFunction(devicesUpdated, 1);
        }
        device->hardwareInterface->powerControl(device->name, deviceStateToInt(device->state));
        Serial.printf("DEVICE_ON high water mark %d\n", uxTaskGetStackHighWaterMark(NULL));
        vTaskDelete(NULL);
      },"DEVICE_ON", 2500, (void *) this, tskIDLE_PRIORITY, &xHandle);
      configASSERT(xHandle);
    }
  }
}
void Device::setStateOff(bool overrideOkToExe) {
  bool willExe = (overrideOkToExe == true) ? true : this->okToExe(false);
  if (willExe) {
  //if (this->okToExe(false) == true) {
    if (this->state != DEVICE_OFF) {
      this->state = DEVICE_OFF;
      TaskHandle_t xHandle = NULL;
      xTaskCreate([](void* pvParameters) {
        Device* device = (Device*) pvParameters;
          Device* devicesUpdated[2];
          devicesUpdated[0] = device; 
          device->postExeFunction(devicesUpdated, 1);
          device->hardwareInterface->powerControl(device->name, deviceStateToInt(device->state));
        
        Serial.printf("DEVICE_OFF high water mark %d\n", uxTaskGetStackHighWaterMark(NULL));
        vTaskDelete(NULL);
      },"DEVICE_OFF", 2500, (void *) this, tskIDLE_PRIORITY, &xHandle);
      configASSERT(xHandle);
    }
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