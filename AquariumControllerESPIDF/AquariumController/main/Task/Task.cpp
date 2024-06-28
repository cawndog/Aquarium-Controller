#include "Task.h"

String Task::getName() {
  return this->name;
}
bool Task::getDisabled() {
  return this->disabled;
}
unsigned long Task::getTime() { //gets task run time interval or scheduled run time
  return this->time;
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

ScheduledTask::ScheduledTask(String name, String shortName, TaskType taskType, AqTaskFunction f) {
  this->name = name;
  this->shortName = shortName;
  this->taskType = taskType;
  this->f = f;
  this->connectedTask = NULL;
  //const char* namePtr = &(this->name[0]);
  /*if(savedState->getBytes(name.c_str(), &(this->settings), sizeof(TaskSettings)) != sizeof(TaskSettings)) {
    this->disabled = true;
    this->time = 0;
    return;
  }*/
  String taskDisabledKey = this->shortName + "_D";
  String taskTimeKey = this->shortName + "_T";
  this->disabled = savedState.getBool(taskDisabledKey.c_str(), true);
  this->time = savedState.getULong(taskTimeKey.c_str(), 0);
  if (!this->getDisabled()) {
    this->determineNextRunTime();
  }
  
}
TimedTask::TimedTask(String name, String shortName, TaskType taskType, AqTaskFunction f) {
  this->name = name;
  this->shortName = shortName;
  this->taskType = taskType;
  this->f = f;
  this->connectedTask = NULL;
  //const char* namePtr = &(this->name[0]);
  /*if(savedState->getBytes(name.c_str(), &(this->settings), sizeof(TaskSettings)) != sizeof(TaskSettings)) {
    this->disabled = true;
    this->time = 100;
    return;
  }*/
  String taskDisabledKey = this->shortName + "_D";
  String taskTimeKey = this->shortName + "_T";
  this->disabled = savedState.getBool(taskDisabledKey.c_str(), true);
  this->time = savedState.getULong(taskTimeKey.c_str(), 0);
  if (!this->getDisabled()) {
    this->determineNextRunTime();
  }
}
void ScheduledTask::initTaskState() { 
  int currentHour = rtc.getHour(true);
  int currentMinute = rtc.getMinute();
  int currentSecond = rtc.getSecond();
  unsigned long secondsSinceStartOfDay = ((currentHour*3600)+(currentMinute*60)+currentSecond); //Seconds since 12:00AM
  if (this->hasConnectedTask() && (this->getDisabled() != true)) {
    if (this->getTime() < this->connectedTask->getTime()) {
      if ((secondsSinceStartOfDay >= this->getTime()) && (secondsSinceStartOfDay < this->connectedTask->getTime())) {
        this->runF();
      }
      else {
        this->connectedTask->runF();
      }
    }
    else {
      if ((secondsSinceStartOfDay >= this->getTime()) || (secondsSinceStartOfDay < this->connectedTask->getTime())) {
        this->runF();
      } 
      else {
        this->connectedTask->runF();
      }
    }
  }
}
void TimedTask::initTaskState() { 
  //do nothing
}
void ScheduledTask::runF() {
  this->f();
}
void ScheduledTask::doTask() {
  if (!this->getDisabled()) {
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
void ScheduledTask::attachConnectedTask(String name, String shortName, AqTaskFunction f){
  this->connectedTask = new ScheduledTask(name, shortName, SCHEDULED_DEVICE_TASK, f);
  this->initTaskState();
}
void TimedTask::attachConnectedTask(String name, String shortName, AqTaskFunction f) {
  return;
}
bool TimedTask::hasConnectedTask() {
  return false;
}
bool ScheduledTask::hasConnectedTask() {
  if (this->connectedTask != NULL) {
    return true;
  }
  return false;
}
void TimedTask::doTask() {
  if (!this->getDisabled()) {
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

  unsigned long currentTime = rtc.getLocalEpoch();
  int currentHour = rtc.getHour(true);
  int currentMinute = rtc.getMinute();
  int currentSecond = rtc.getSecond();
  unsigned long secondsSinceStartOfDay = ((currentHour*3600)+(currentMinute*60)+currentSecond); //Seconds since 12:00AM
  if (secondsSinceStartOfDay <= this->getTime()) { //Time of day is before the task's run time
    unsigned long secondsUntilRunTime = this->getTime() - secondsSinceStartOfDay;
    this->nextRunTime = currentTime + secondsUntilRunTime;
  } else {
    //Time of day is past this task's run time. Schedule task for tomorrow.
    unsigned long remainingSecsInDay = 86400 - secondsSinceStartOfDay;
    this->nextRunTime = currentTime + remainingSecsInDay + this->getTime();
  }
}
void TimedTask::determineNextRunTime() {
  unsigned long currentTime = rtc.getLocalEpoch();
  this->nextRunTime = currentTime + this->getTime();
}
void ScheduledTask::updateSettings(bool disabled, unsigned long time) {
  this->disabled = disabled; 
  //this->time = static_cast<uint16_t>(time);
  this->time = time;
  //const char* namePtr = &(this->name[0]);
  //this->savedState->putBytes(name.c_str(), &(this->settings), sizeof(TaskSettings));
  String taskDisabledKey = this->shortName + "_D";
  String taskTimeKey = this->shortName + "_T";
  savedState.putBool(taskDisabledKey.c_str(), disabled);
  savedState.putULong(taskTimeKey.c_str(), time);
  if (!this->getDisabled()) {
    this->determineNextRunTime();
    if (this->hasConnectedTask()) {
      this->initTaskState();
    }
  }
}
void TimedTask::updateSettings(bool disabled, unsigned long time) {
  this->disabled = disabled; 
  this->time = time;
  //const char* namePtr = &(this->name[0]);
  //this->savedState->putBytes(name.c_str(), &(this->settings), sizeof(TaskSettings));
  String taskDisabledKey = this->shortName + "_D";
  String taskTimeKey = this->shortName + "_T";
  savedState.putBool(taskDisabledKey.c_str(), disabled);
  savedState.putULong(taskTimeKey.c_str(), time);
  if (!this->getDisabled()) {
    this->determineNextRunTime();
  }
}