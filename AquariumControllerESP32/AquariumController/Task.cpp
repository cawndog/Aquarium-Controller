#include "Task.h"

String Task::getName() {
  return this->name;
}
bool Task::getDisabled() {
  return this->settings.disabled;
}
unsigned long Task::getTime() { //gets task run time interval or scheduled run time
  return this->settings.time;
}
void Task::setPrivate() {
  this->privateTask = true; 
}

ScheduledTask::ScheduledTask(String name, TaskType taskType, Preferences* savedState, ESP32Time* rtc, void (*Taskfunc), Device* device) {
  this->name = name;
  this->taskType = taskType;
  this->device = device;
  //this->deviceFunc = deviceFunc;
  this->savedState = savedState;
  const char* namePtr = &(this->name[0]);
  if(savedState->getBytes(namePtr, &(this->settings), sizeof(this->settings)) != sizeof(this->settings)) {
    this->settings.disabled = true;
    this->settings.time = 0;
    return;
  }
  this->determineNextRunTime();
}
TimedTask::TimedTask(String name, TaskType taskType, Preferences* savedState, ESP32Time* rtc, void (*Taskfunc), Device* device, Sensor* sensor) {
  this->name = name;
  this->taskType = taskType;
  this->savedState = savedState;
  const char* namePtr = &(this->name[0]);
  if(savedState->getBytes(namePtr, &(this->settings), sizeof(this->settings)) != sizeof(this->settings)) {
    this->settings.disabled = true;
    this->settings.time = 100;
    return;
  }
  this->determineNextRunTime();
}
void ScheduledTask::doTask() {
  if (!this->settings.disabled) {
    switch(this->taskType) {
      case TDEVICE_ON: {
        this->device->setStateOn();
        break;
      }
      case TDEVICE_OFF: {
        this->device->setStateOff();
        break;
      }
      default: 
        return;
    };
    this->determineNextRunTime();
  }
  return;
}
void TimedTask::doTask() {
  if (!this->settings.disabled) {
    switch(this->taskType) {
      case SENSOR_READ: {
        this->sensor->readSensor();
        break;
      }
      case TDEVICE_ON: {
        this->device->setStateOn();
        break;
      }
      case TDEVICE_OFF: {
        this->device->setStateOff();
        break;
      }
      default: 
        return;
    };
    this->determineNextRunTime();
  }
  return;
}
void ScheduledTask::determineNextRunTime() {
  unsigned long currentTime = rtc->getLocalEpoch();
  int currentHour = rtc->getHour(true);
  int currentMinute = rtc->getMinute();
  int currentSecond = rtc->getSecond();
  unsigned long secondsSinceStartOfDay = ((currentHour*3600)+(currentMinute*60)+currentSecond); //Seconds since 12:00AM
  if (secondsSinceStartOfDay <= (this->settings.time*60)) { //Time of day is before the task's run time
    unsigned long remainingSecs = secondsSinceStartOfDay - (this->settings.time*60);
    this->nextRunTime = currentTime + remainingSecs;
  } else {
    //Time of day is past this task's run time. Schedule task for tomorrow.
    unsigned long remainingSecsInDay = 86400 - secondsSinceStartOfDay;
    this->nextRunTime = currentTime + remainingSecsInDay + (this->settings.time*60);
  }
}
void TimedTask::determineNextRunTime() {
  unsigned long currentTime = rtc->getLocalEpoch();
  this->nextRunTime = currentTime + this->settings.time;
}
void ScheduledTask::updateSettings(bool disabled, unsigned long time) {
  this->settings.disabled = disabled; 
  this->settings.time = static_cast<uint16_t>(time);
  const char* namePtr = &(this->name[0]);
  this->savedState->putBytes(namePtr, &(this->settings), sizeof(this->settings));
}
void TimedTask::updateSettings(bool disabled, unsigned long time) {
  this->settings.disabled = disabled; 
  this->settings.time = time;
  const char* namePtr = &(this->name[0]);
  this->savedState->putBytes(namePtr, &(this->settings), sizeof(this->settings));

}