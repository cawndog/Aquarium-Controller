#include "Task.h"

String Task::getName() {
  return this->name;
}
bool Task::getEnabled() {
  return this->enabled;
}
void Task::scheduleTaskToRun() {
  if (this->getEnabled()) {
    xTaskCreate([](void* pvParameters) {
      Task* thisTask = (Task*) pvParameters;
      TickType_t xLastWakeTime = xTaskGetTickCount();
      while (thisTask->getEnabled()) {
        unsigned long secsUntilRunTime = thisTask->nextRunTime - rtc.getLocalEpoch();
        thisTask->sleepUntil(&xLastWakeTime, secsUntilRunTime);
        thisTask->doTask();
        Serial.printf("TASK_SCHEDULER high water mark %d\n", uxTaskGetStackHighWaterMark(NULL));
      }
      vTaskDelete(NULL);
    },"TASK_SCHEDULER", 2500, (void *) this, tskIDLE_PRIORITY, &xHandle);
  }
}
void Task::unscheduleTask() {
  if (this->xHandle != NULL) {
    vTaskDelete(this->xHandle);
    xHandle = NULL;
  }
}
void Task::sleepUntil( TickType_t *prevWakeTime,  unsigned int sec ) {
  //TickType_t xLastWakeTime = xTaskGetTickCount ();
  TickType_t xDelay = sec * 1000 / portTICK_PERIOD_MS;
  while(xDelay >= portMAX_DELAY ) {
      xTaskDelayUntil(prevWakeTime, portMAX_DELAY-1);
      xDelay -= portMAX_DELAY-1;
  }
  xTaskDelayUntil(prevWakeTime, xDelay);
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
    this->scheduleTaskToRun();
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
    this->scheduleTaskToRun();
  }
}
void ScheduledTask::initTaskState() { 
  int currentHour = rtc.getHour(true);
  int currentMinute = rtc.getMinute();
  int currentSecond = rtc.getSecond();
  unsigned long secondsSinceStartOfDay = ((currentHour*3600)+(currentMinute*60)+currentSecond); //Seconds since 12:00AM
  //Only init task state from the parent task. Parent task determines whether itself or its connectedTask needs to run.
  if ((this->hasConnectedTask()) && (this->getEnabled() == true)) {
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
  this->f();
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
  //Unschedule task. Init task state, if necessary. 
  //Then determine its next run time and schedule task again if its enabled. 
  this->unscheduleTask();
  if (this->getEnabled() == true) {
    if (this->hasConnectedTask()) {
      this->initTaskState();
    }
    this->determineNextRunTime();
    this->scheduleTaskToRun();
  }
}
void TimedTask::updateSettings(bool enabled, unsigned long time) {
  this->enabled = enabled; 
  this->time = time;
  String taskEnabledKey = this->shortName + "_D";
  String taskTimeKey = this->shortName + "_T";
  savedState.putBool(taskEnabledKey.c_str(), enabled);
  savedState.putULong(taskTimeKey.c_str(), time);
  //Unschedule task, run task's function now. If task is enabled, determine next run time and schedule task.
  this->unscheduleTask();
  if (this->getEnabled()) {
    this->f();
    this->determineNextRunTime();
    this->scheduleTaskToRun();
  } 
}