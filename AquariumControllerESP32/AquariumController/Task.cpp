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

ScheduledTask::ScheduledTask(String name, TaskType taskType, Preferences* savedState, AqTaskFunction f) {
  this->name = name;
  this->taskType = taskType;
  this->f = f;
  this->savedState = savedState;
  this->connectedTask = NULL;
  //const char* namePtr = &(this->name[0]);
  /*if(savedState->getBytes(name.c_str(), &(this->settings), sizeof(TaskSettings)) != sizeof(TaskSettings)) {
    this->disabled = true;
    this->time = 0;
    return;
  }*/
  String taskDisabledKey = this->name + "_D";
  String taskTimeKey = this->name + "_T";
  this->disabled = this->savedState->getBool(taskDisabledKey.c_str(), true);
  this->time = this->savedState->getULong(taskTimeKey.c_str(), 0);
  if (!this->getDisabled()) {
    this->determineNextRunTime();
  }
  
}
TimedTask::TimedTask(String name, String shortname, TaskType taskType, Preferences* savedState, AqTaskFunction f) {
  this->name = name;
  this->shortName = shortName;
  this->taskType = taskType;
  this->f = f;
  this->savedState = savedState;
  this->connectedTask = NULL;
  //const char* namePtr = &(this->name[0]);
  /*if(savedState->getBytes(name.c_str(), &(this->settings), sizeof(TaskSettings)) != sizeof(TaskSettings)) {
    this->disabled = true;
    this->time = 100;
    return;
  }*/
  String taskDisabledKey = this->shortName + "_D";
  String taskTimeKey = this->shortName + "_T";
  this->disabled = this->savedState->getBool(taskDisabledKey.c_str(), true);
  this->time = this->savedState->getULong(taskTimeKey.c_str(), 0);
  if (!this->getDisabled()) {
    this->determineNextRunTime();
  }
}
void ScheduledTask::initTaskState() { 
  Serial.printf("DEBUG: in initTaskState() for task: %s\n.", this->getName());
  int currentHour = rtc.getHour(true);
  int currentMinute = rtc.getMinute();
  int currentSecond = rtc.getSecond();
  unsigned long secondsSinceStartOfDay = ((currentHour*3600)+(currentMinute*60)+currentSecond); //Seconds since 12:00AM
  Serial.println("DEBUG: in initTaskState(). Time data acquired.");
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
  Serial.println("DEBUG: in initTaskState(). Function completed successfully.");
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
void ScheduledTask::attachConnectedTask(String name, AqTaskFunction f){
  this->connectedTask = new ScheduledTask(name, SCHEDULED_DEVICE_TASK, savedState, f);
  this->initTaskState();
}
void TimedTask::attachConnectedTask(String name, AqTaskFunction f) {
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
  Serial.printf("DEBUG: in determineNextRunTime() for ScheduledTask %s.\n", this->getName().c_str());
  unsigned long currentTime = rtc.getLocalEpoch();
  Serial.printf("DEBUG: Alive after getLocalEpoch\n");
  int currentHour = rtc.getHour(true);
  Serial.printf("DEBUG: Alive after getHour\n");
  int currentMinute = rtc.getMinute();
  Serial.printf("DEBUG: Alive after getMinute\n");
  int currentSecond = rtc.getSecond();
  Serial.printf("DEBUG: Alive after getSecond\n");
  unsigned long secondsSinceStartOfDay = ((currentHour*3600)+(currentMinute*60)+currentSecond); //Seconds since 12:00AM
  if (secondsSinceStartOfDay <= this->getTime()) { //Time of day is before the task's run time
    unsigned long secondsUntilRunTime = this->getTime() - secondsSinceStartOfDay;
    this->nextRunTime = currentTime + secondsUntilRunTime;
  } else {
    //Time of day is past this task's run time. Schedule task for tomorrow.
    unsigned long remainingSecsInDay = 86400 - secondsSinceStartOfDay;
    this->nextRunTime = currentTime + remainingSecsInDay + this->getTime();
  }
  Serial.printf("DEBUG: in determineNextRunTime() Finished successfully.\n");
}
void TimedTask::determineNextRunTime() {
  Serial.printf("DEBUG: in determineNextRunTime() for TimedTask %s.\n", this->getName().c_str());
  unsigned long currentTime = rtc.getLocalEpoch();
  this->nextRunTime = currentTime + this->getTime();
  Serial.printf("DEBUG: in determineNextRunTime() Finished successfully.\n");
}
void ScheduledTask::updateSettings(bool disabled, unsigned long time) {
  Serial.printf("DEBUG: in updateSettings for %s. disabled: %d time: %d\n",this->getName().c_str(), disabled, time);
  this->disabled = disabled; 
  //this->time = static_cast<uint16_t>(time);
  this->time = time;
  //const char* namePtr = &(this->name[0]);
  //this->savedState->putBytes(name.c_str(), &(this->settings), sizeof(TaskSettings));
  String taskDisabledKey = this->name + "_D";
  String taskTimeKey = this->name + "_T";
  Serial.printf("DEBUG: in updateSettings. About to save state.\n");
  this->savedState->putBool(taskDisabledKey.c_str(), disabled);
  this->savedState->putULong(taskTimeKey.c_str(), time);
  if (!this->getDisabled()) {
    this->determineNextRunTime();
    if (this->hasConnectedTask()) {
      this->initTaskState();
    }
  }
  Serial.printf("DEBUG: in updateSettings. Finished successfully.\n");
}
void TimedTask::updateSettings(bool disabled, unsigned long time) {
  Serial.printf("DEBUG: in updateSettings() for %s. disabled: %d time: %d\n",this->getName().c_str(), disabled, time);
  this->disabled = disabled; 
  this->time = time;
  //const char* namePtr = &(this->name[0]);
  //this->savedState->putBytes(name.c_str(), &(this->settings), sizeof(TaskSettings));
  String taskDisabledKey = this->shortName + "_D";
  String taskTimeKey = this->shortName + "_T";
  this->savedState->putBool(taskDisabledKey.c_str(), disabled);
  this->savedState->putULong(taskTimeKey.c_str(), time);
  if (!this->getDisabled()) {
    this->determineNextRunTime();
  }
}