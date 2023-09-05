#include "AqController.h"

AqController::AqController() {
  for (int i = 0; i < NUM_TASKS; i++) {
    tasks[i] = NULL;
  }
}
void AqController::init(AqWebServerInterface* aqWebServerInterface) {
    //Setup real time clock
  this->aqWebServerInterface = aqWebServerInterface;
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  while (!getLocalTime(&timeinfo)){
    //Wait until the local time is determined.
  }
  rtc.setTimeStruct(timeinfo);
  //rtc.setTime(30, 24, 15, 17, 1, 2021);  // 17th Jan 2021 15:24:30
  savedState.begin("aqController", false);
  this->aqThermostat = savedState.getShort("aqThermostat", 82);
  hardwareInterface.init(&savedState);
  heater.init("Heater", &hardwareInterface, [this](Device** devices, int numDevices) {
    this->aqWebServerInterface->deviceStateUpdate(devices, numDevices);
  });
  lights.init("Lights", &hardwareInterface, [this](Device** devices, int numDevices) {
    this->aqWebServerInterface->deviceStateUpdate(devices, numDevices);
  });
  co2.init("CO2", &hardwareInterface, [this](Device** devices, int numDevices) {
    this->aqWebServerInterface->deviceStateUpdate(devices, numDevices);
  });
  airPump.init("Air Pump", &hardwareInterface, [this](Device** devices, int numDevices) {
    this->aqWebServerInterface->deviceStateUpdate(devices, numDevices);
  });
  filter.init("Filter", &hardwareInterface, [this](Device** devices, int numDevices) {
    this->aqWebServerInterface->deviceStateUpdate(devices, numDevices);
  });
  co2.attachConnectedDevice(&airPump);
  airPump.attachConnectedDevice(&co2);
  aqTemperature.init("Aquarium Temperature", &hardwareInterface, [this](Sensor* sensor) {
    if (this->maintMode != true) {
      float valAsFloat = sensor->getValue().toFloat();
      if (valAsFloat < (aqThermostat - 0.5)) {
        heater.setStateOn();
      }
      else if (valAsFloat > (aqThermostat + 0.5)) {
        heater.setStateOff();
      }
    }
    if (sensor->prevValue != sensor->value) {
      this->aqWebServerInterface->sensorReadingUpdate(sensor);
    }
  });
  tds.init("TDS", &hardwareInterface, &aqTemperature, [this](Sensor* sensor) {
    if (sensor->prevValue != sensor->value) {
      this->aqWebServerInterface->sensorReadingUpdate(sensor);
    }
  });
  aqTemperature.readSensor();
  tds.readSensor();
  tasks[0] = new ScheduledTask("Lights On", SCHEDULED_DEVICE_TASK, &savedState, &rtc, [&]() {
    lights.setStateOn();
  });
  tasks[0]->connectedTask = new ScheduledTask("Lights Off", SCHEDULED_DEVICE_TASK, &savedState, &rtc, [&]() {
    lights.setStateOff();
  });
  tasks[1] = new ScheduledTask("CO2 On", SCHEDULED_DEVICE_TASK, &savedState, &rtc, [&]() {
    co2.setStateOn();
  });
  tasks[1]->connectedTask = new ScheduledTask("CO2 Off", SCHEDULED_DEVICE_TASK, &savedState, &rtc, [&]() {
    co2.setStateOff();
  });
  tasks[2] = new ScheduledTask("Air Pump On", SCHEDULED_DEVICE_TASK, &savedState, &rtc, [&]() {
    airPump.setStateOn();
  });
  tasks[2]->connectedTask = new ScheduledTask("Air Pump Off", SCHEDULED_DEVICE_TASK, &savedState, &rtc, [&]() {
    airPump.setStateOff();
  });
  //tasks[3] = new TimedTask ("Read Aquarium Temp", SENSOR_READ, &savedState, &rtc, NULL, NULL, &aqTemperature);
  //tasks[4] = new TimedTask ("Read TDS", SENSOR_READ, &savedState, &rtc, [&](){}, NULL, &tds);
  tasks[3] = new TimedTask ("Read Aquarium Temp", TIMED_TASK, &savedState, &rtc, [&](){
    aqTemperature.readSensor();
  });
  tasks[4] = new TimedTask ("Read TDS", TIMED_TASK, &savedState, &rtc, [&](){
    tds.readSensor();
  });
  tasks[5] = new TimedTask ("Check WiFi Connection", TIMED_TASK, &savedState, &rtc, [&](){
    if (WiFi.status() != WL_CONNECTED) {
      WiFi.reconnect();
    }
  });
  tasks[6] = new TimedTask ("Update Dynamic IP", TIMED_TASK, &savedState, &rtc, [&](){
    aqWebServerInterface->updateDynamicIP();
  });
  //initScheduledDeviceTaskStates();
}

Task* AqController::getTaskByName(String name) {
  if (tasks == NULL) {
    return NULL;
  }  
  for(int i = 0; tasks[i] != NULL; i++) {
    if (tasks[i]->getName() == name){
      return tasks[i];
    }  
    if (tasks[i]->connectedTask != NULL) {
      if (tasks[i]->connectedTask->getName() == name)
        return tasks[i]->connectedTask;
    }
  }
  return NULL;
}
void AqController::initScheduledDeviceTaskStates() {
  if (tasks == NULL) {
    return;
  }
  for (int i = 0; tasks[i] != NULL; i++) {
    if (tasks[i]->taskType == SCHEDULED_DEVICE_TASK) {
      tasks[i]->initTaskState();
    }
  }
}
void AqController::setNextTaskWithEvent() {
  if (tasks == NULL) {
    return;
  }
  Task* nextTaskWithEvent = tasks[0];
  for (int i = 0; tasks[i] != NULL; i++) {
    if (tasks[i]->getDisabled()) {
      continue;
    }
    if (tasks[i]->connectedTask != NULL) {
      if (tasks[i]->connectedTask->nextRunTime < nextTaskWithEvent->nextRunTime) {
        nextTaskWithEvent = tasks[i]->connectedTask;
      }
    }
    if (tasks[i]->nextRunTime < nextTaskWithEvent->nextRunTime) {
      nextTaskWithEvent = tasks[i];
    }
  }
  if (nextTaskWithEvent != NULL) { 
    if (nextTaskWithEvent->getDisabled() == true) {
      nextTaskWithEvent = NULL;
    }
  }
}
void AqController::scheduleNextTask() {
  //timerAlarmWrite(aqController.taskTimer, 20000, true);
  //timerAlarmEnable(aqController.taskTimer);
  if (tasks == NULL) {
    return;
  }
  #ifdef useSerial 
    Serial.println("Scheduling next task.");
  #endif
  #ifdef useSerialBT
    SerialBT.println("Scheduling next task.");
  #endif
  setNextTaskWithEvent();
  if (nextTaskWithEvent != NULL) { 
    unsigned long currentLocalEpoch = rtc.getLocalEpoch();
    timerRestart(taskTimer);

    if (nextTaskWithEvent->nextRunTime <= currentLocalEpoch) {
      timerAlarmWrite(taskTimer, 500, false);
    }
    else {
      unsigned long secondsUntilNextEvent = nextTaskWithEvent->nextRunTime - currentLocalEpoch;

      timerAlarmWrite(taskTimer, secondsUntilNextEvent * 2000, false);
    }
    
    timerAlarmEnable(taskTimer);
  }
 
}
Device* AqController::getDeviceByName(String devName) {
  for (int i = 0; i < sizeof(devices) / sizeof(Device*); i++) {
    if (devices[i]->name == devName) {
      return devices[i];
    }
  }
  return NULL;
}
Sensor* AqController::getSensorByName(String sensorName) {
  for (int i = 0; i < sizeof(sensors) / sizeof(Sensor*); i++) {
    if (sensors[i]->name == sensorName) {
      return sensors[i];
    }
  }
  return NULL;
}

