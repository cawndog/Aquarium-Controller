#include "AqController.h"

AqController::AqController() {
  for (int i = 0; i < NUM_TASKS; i++) {
    tasks[i] = NULL;
  }
}
void AqController::init(AqWebServerInterface* aqWebServerInterface) {
    //Setup real time clock
  this->aqWebServerInterface = aqWebServerInterface;

  //rtc.setTime(30, 24, 15, 17, 1, 2021);  // 17th Jan 2021 15:24:30
  
  this->aqThermostat = savedState.getShort("aqThermostat", 82);
  hardwareInterface.init();
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
  tasks[0] = new ScheduledTask("Lights On", SCHEDULED_DEVICE_TASK, [this]() {
    lights.setStateOn();
  });
  tasks[0]->attachConnectedTask("Lights Off", [this]() {
    lights.setStateOff();
  });
  tasks[1] = new ScheduledTask("CO2 On", SCHEDULED_DEVICE_TASK, [this]() {
    co2.setStateOn();
  });
  tasks[1]->attachConnectedTask("CO2 Off", [this]() {
    co2.setStateOff();
  });
  tasks[2] = new ScheduledTask("Air Pump On", SCHEDULED_DEVICE_TASK, [this]() {
    airPump.setStateOn();
  });
  tasks[2]->attachConnectedTask("Air Pump Off", [this]() {
    airPump.setStateOff();
  });
  //tasks[3] = new TimedTask ("Read Aquarium Temp", SENSOR_READ, NULL, NULL, &aqTemperature);
  //tasks[4] = new TimedTask ("Read TDS", SENSOR_READ, [&](){}, NULL, &tds);
  tasks[3] = new TimedTask ("Read Aquarium Temp", "Rd_Aq_Tmp", TIMED_TASK, [this](){
    aqTemperature.readSensor();
  });
  tasks[4] = new TimedTask ("Read TDS", "Rd_Tds", TIMED_TASK, [this](){
    tds.readSensor();
  });
  tasks[5] = new TimedTask ("Check WiFi Connection", "Ck_Wifi", TIMED_TASK, [this](){
    if (WiFi.status() != WL_CONNECTED) {
      WiFi.reconnect();
    }       
  });
  tasks[6] = new TimedTask ("Update Dynamic IP", "Up_Dyn_IP", TIMED_TASK, [this](){
    this->aqWebServerInterface->updateDynamicIP();
  });
}

Task* AqController::getTaskByName(String name) {
  if (tasks == NULL) {
    return NULL;
  }  
  for(int i = 0; tasks[i] != NULL; i++) {
    if (tasks[i]->getName() == name){
      return tasks[i];
    }  
    if (tasks[i]->hasConnectedTask()) {
      if (tasks[i]->connectedTask->getName() == name)
        return tasks[i]->connectedTask;
    }
  }
  return NULL;
}
void AqController::setNextTaskWithEvent() {
  if (tasks == NULL) {
    return;
  }
  Task* nextTaskWithEventLocal = tasks[0];
  for (int i = 0; tasks[i] != NULL; i++) {
    //Serial.printf("Checking tasks[%d]. Task name: %s\n", i, tasks[i]->getName().c_str());
    if (tasks[i]->getDisabled()) {
      //Serial.printf("tasks[%d]. Task name: %s is disabled. Continuing.\n", i, tasks[i]->getName().c_str());
      continue;
    }
    if (tasks[i]->hasConnectedTask()) {
      //Serial.printf("Checking task[%d]'s connectedTask. Task name: %s\n", i, tasks[i]->connectedTask->getName().c_str());
      //Task* connectedTask = tasks[i]->connectedTask;
      if (tasks[i]->connectedTask->nextRunTime < nextTaskWithEventLocal->nextRunTime) {
        //Serial.printf("Setting nextTaskWithEventLocal to tasks[%d]->connectedTask. Task name: %s\n", i, tasks[i]->connectedTask->getName().c_str());
        //Serial.printf("nextTaskWithEventLocal = %s Time: %d  -->  %s Time: %d\n", nextTaskWithEventLocal->getName().c_str(), nextTaskWithEventLocal->nextRunTime, tasks[i]->connectedTask->getName().c_str(), tasks[i]->connectedTask->nextRunTime);
        nextTaskWithEventLocal = tasks[i]->connectedTask;
      }
    }
    if (tasks[i]->nextRunTime < nextTaskWithEventLocal->nextRunTime) {
      //Serial.printf("Setting nextTaskWithEventLocal to tasks[%d]. Task name: %s\n", i, tasks[i]->getName().c_str());
      //Serial.printf("nextTaskWithEventLocal = %s Time: %d  -->  %s Time: %d\n", nextTaskWithEventLocal->getName().c_str(), nextTaskWithEventLocal->nextRunTime, tasks[i]->getName().c_str(), tasks[i]->nextRunTime);
      nextTaskWithEventLocal = tasks[i];
    }
  }
  if (nextTaskWithEventLocal != NULL) { 
    if (nextTaskWithEventLocal->getDisabled() == true) {
      //Serial.print("nextTaskWithEventLocal was disabled. Setting nextTaskWithEventLocal to NULL.");
      nextTaskWithEventLocal = NULL;
    }
  }
  this->nextTaskWithEvent = nextTaskWithEventLocal;

}
void AqController::scheduleNextTask() {

  //timerAlarmWrite(aqController.taskTimer, 20000, true);
  //timerAlarmEnable(aqController.taskTimer);
  if (tasks == NULL) {
    return;
  }

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
  } else {
    //Serial.println("nextTaskWithEvent was NULL.");
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
void AqController::setAqWebServerInterface(AqWebServerInterface* aqWebServerInterface) {
  this->aqWebServerInterface = aqWebServerInterface;
}

