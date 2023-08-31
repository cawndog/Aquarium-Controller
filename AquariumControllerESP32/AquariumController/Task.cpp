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
String Task::taskTypeToString() {
  switch (this->taskType) {
    case SCHEDULED_TASK: {
      return String("SCHEDULED_TASK");
      break;
    }
    case SCHEDULED_DEVICE_TASK: {
      return String("SCHEDULED_DEVICE_TASK");
      break;
    }
    case TIMED_TASK: {
      return String("TIMED_TASK");
      break;
    }
    default: {
      return String("");
    }
  }
}

ScheduledTask::ScheduledTask(String name, TaskType taskType, Preferences* savedState, ESP32Time* rtc, AqTaskFunction f) {
  this->name = name;
  this->taskType = taskType;
  this->device = device;
  //this->deviceFunc = deviceFunc;
  this->f = f;
  this->savedState = savedState;
  const char* namePtr = &(this->name[0]);
  if(savedState->getBytes(namePtr, &(this->settings), sizeof(this->settings)) != sizeof(this->settings)) {
    this->settings.disabled = true;
    this->settings.time = 0;
    return;
  }
  if (!this->settings.disabled) {
    this->determineNextRunTime();
  }
  
}
TimedTask::TimedTask(String name, TaskType taskType, Preferences* savedState, ESP32Time* rtc, AqTaskFunction f) {
  this->name = name;
  this->taskType = taskType;
  this->sensor = sensor;
  this->f = f;
  this->savedState = savedState;
  const char* namePtr = &(this->name[0]);
  if(savedState->getBytes(namePtr, &(this->settings), sizeof(this->settings)) != sizeof(this->settings)) {
    this->settings.disabled = true;
    this->settings.time = 100;
    return;
  }
  if (!this->settings.disabled) {
    this->determineNextRunTime();
  }
}
void ScheduledTask::initTaskState() { 
  int currentHour = rtc->getHour(true);
  int currentMinute = rtc->getMinute();
  int currentSecond = rtc->getSecond();
  unsigned long secondsSinceStartOfDay = ((currentHour*3600)+(currentMinute*60)+currentSecond); //Seconds since 12:00AM
  if (this->connectedTask != NULL && !this->getDisabled()) {
    if (this->settings.time < this->connectedTask->settings.time) {
      if (secondsSinceStartOfDay >= (this->settings.time) && secondsSinceStartOfDay < (this->connectedTask->settings.time)) {
        this->f();
      }
      else {
        this->connectedTask->runF();
      }
    }
    else {
      if (secondsSinceStartOfDay >= (this->settings.time) || secondsSinceStartOfDay < this->connectedTask->settings.time) {
        this->f();
      } 
      else {
        this->connectedTask->runF();
      }
    }
  }
}
void TimedTask::initTaskState() {}
void ScheduledTask::runF() {
  this->f();
}
void ScheduledTask::doTask() {
  if (!this->settings.disabled) {
    this->f();
    /*
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
    };*/
    this->determineNextRunTime();
  }
  return;
}
void TimedTask::runF() {
  //this->f();
}
void TimedTask::doTask() {
  if (!this->settings.disabled) {
    /*switch(this->taskType) {
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
    };*/
    this->f();
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
  if (secondsSinceStartOfDay <= (this->settings.time)) { //Time of day is before the task's run time
    unsigned long remainingSecs = secondsSinceStartOfDay - (this->settings.time);
    this->nextRunTime = currentTime + remainingSecs;
  } else {
    //Time of day is past this task's run time. Schedule task for tomorrow.
    unsigned long remainingSecsInDay = 86400 - secondsSinceStartOfDay;
    this->nextRunTime = currentTime + remainingSecsInDay + (this->settings.time);
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
  if (!this->settings.disabled) {
    this->determineNextRunTime();
  }
}
void TimedTask::updateSettings(bool disabled, unsigned long time) {
  this->settings.disabled = disabled; 
  this->settings.time = time;
  const char* namePtr = &(this->name[0]);
  this->savedState->putBytes(namePtr, &(this->settings), sizeof(this->settings));
  if (!this->settings.disabled) {
    this->determineNextRunTime();
  }
}