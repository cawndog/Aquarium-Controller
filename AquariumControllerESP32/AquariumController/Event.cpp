#include "Event.h"

ScheduledTask::doTask() {
  if (!this->disabled) {
    switch(this->taskType) {
      case DEVICE_ON: {
        this->device->setStateOn();
        return;
      }
      case DEVICE_OFF: {
        this->device->setStateOff();
        return;
      }
      case default: 
        return;
    };
  }
}
TimedTask::doTask() {
  if (!this->disabled) {
    switch(this->taskType) {
      case SENSOR_READ: {
        this->sensor->readSensor();
        return;
      }
      case DEVICE_ON: {
        this->device->setStateOn();
        return;
      }
      case DEVICE_OFF: {
        this->device->setStateOff();
        return;
      }
      case default: 
        return;
    };
  }
}