#include "Task.h"

String Task::getName() {
  return this->name;
}
bool Task::getEnabled() {
  return this->enabled;
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
  String taskEnabledKey = this->shortName + "_D";
  String taskTimeKey = this->shortName + "_T";
  this->enabled = savedState.getBool(taskEnabledKey.c_str(), false);
  this->time = savedState.getULong(taskTimeKey.c_str(), 0);
  if (this->getEnabled()) {
    this->determineNextRunTime();
  }
  
}
TimedTask::TimedTask(String name, String shortName, TaskType taskType, AqTaskFunction f) {
  this->name = name;
  this->shortName = shortName;
  this->taskType = taskType;
  this->f = f;
  this->connectedTask = NULL;
  String taskEnabledKey = this->shortName + "_D";
  String taskTimeKey = this->shortName + "_T";
  this->enabled = savedState.getBool(taskEnabledKey.c_str(), false);
  this->time = savedState.getULong(taskTimeKey.c_str(), 0);
  if (this->getEnabled()) {
    this->determineNextRunTime();
  }
}
void ScheduledTask::initTaskState() { 
  int currentHour = rtc.getHour(true);
  int currentMinute = rtc.getMinute();
  int currentSecond = rtc.getSecond();
  unsigned long secondsSinceStartOfDay = ((currentHour*3600)+(currentMinute*60)+currentSecond); //Seconds since 12:00AM
  if (this->hasConnectedTask() && (this->getEnabled() == true)) {
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
  if (this->getEnabled()) {
    this->f();
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
  if (this->getEnabled() == true) {
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
void ScheduledTask::updateSettings(bool enabled, unsigned long time) {
  this->enabled = enabled; 
  this->time = time;
  String taskEnabledKey = this->shortName + "_D";
  String taskTimeKey = this->shortName + "_T";
  savedState.putBool(taskEnabledKey.c_str(), enabled);
  savedState.putULong(taskTimeKey.c_str(), time);
  if (this->getEnabled() == true) {
    this->determineNextRunTime();
    if (this->hasConnectedTask()) {
      this->initTaskState();
    }
  }
}
void TimedTask::updateSettings(bool enabled, unsigned long time) {
  this->enabled = enabled; 
  this->time = time;
  String taskEnabledKey = this->shortName + "_D";
  String taskTimeKey = this->shortName + "_T";
  savedState.putBool(taskEnabledKey.c_str(), enabled);
  savedState.putULong(taskTimeKey.c_str(), time);
  if (this->getEnabled()) {
    this->determineNextRunTime();
  }
}