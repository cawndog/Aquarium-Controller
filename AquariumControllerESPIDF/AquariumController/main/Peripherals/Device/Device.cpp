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
  TaskHandle_t xHandle = NULL;
  xTaskCreate([](void* pvParameters) {
    Device* device = (Device*) pvParameters;
    if (device->state != DEVICE_ON) {
      device->state = DEVICE_ON; 
      Device* devicesUpdated[2];
      devicesUpdated[0] = device;
      if (device->connectedDevice != NULL) {
        if(device->connectedDevice->state == DEVICE_ON) {
          device->connectedDevice->state = DEVICE_OFF;
          devicesUpdated[1] = device->connectedDevice;
          device->webSocketUpdateState(devicesUpdated, 2);
        }
        else {
          device->webSocketUpdateState(devicesUpdated, 1);
        }     
      }else {
        device->webSocketUpdateState(devicesUpdated, 1);
      }
      device->hardwareInterface->powerControl(device->name, deviceStateToInt(device->state));
    }
    Serial.printf("DEVICE_ON high water mark %d\n", uxTaskGetStackHighWaterMark(NULL));
    vTaskDelete(NULL);
  },"DEVICE_ON", 2500, (void *) this, tskIDLE_PRIORITY, &xHandle);
  //},"DEVICE_ON", configMINIMAL_STACK_SIZE, (void *) this, tskIDLE_PRIORITY, &xHandle);
  configASSERT(xHandle);
}
void Device::setStateOff() {
  TaskHandle_t xHandle = NULL;
  xTaskCreate([](void* pvParameters) {
    Device* device = (Device*) pvParameters;
    if (device->state != DEVICE_OFF) {
      device->state = DEVICE_OFF;
      Device* devicesUpdated[2];
      devicesUpdated[0] = device; 
      device->webSocketUpdateState(devicesUpdated, 1);
      device->hardwareInterface->powerControl(device->name, deviceStateToInt(device->state));
    }
    Serial.printf("DEVICE_OFF high water mark %d\n", uxTaskGetStackHighWaterMark(NULL));
    vTaskDelete(NULL);
  },"DEVICE_ON", 2500, (void *) this, tskIDLE_PRIORITY, &xHandle);
  //},"DEVICE_OFF", configMINIMAL_STACK_SIZE, (void *) this, tskIDLE_PRIORITY, &xHandle);
  configASSERT(xHandle);
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