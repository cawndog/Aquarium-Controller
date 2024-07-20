#include "AqController.h"

AqController::AqController() {
  for (int i = 0; i < NUM_TASKS; i++) {
    tasks[i] = NULL;
  }
}
void AqController::init(AqWebServerInterface* aqWebServerInterface) {
    //Setup real time clock
  this->aqWebServerInterface = aqWebServerInterface;
  //this->aqThermostat = savedState.getShort("aqThermostat", 82);
  hardwareInterface.init();
  //***************** Init Devices *****************
  //Each device init takes two callback functions. 
  waterValve.init("Water Valve", &hardwareInterface, 
    [this](bool newState){
      if (newState == true) {
        return (this->waterSensorAlarm.getAlarmState() == 0) ? true : false;
      }
      return true;
    }, [this](Device** devices, int numDevices) {
      this->aqWebServerInterface->deviceStateUpdate(devices, numDevices);
  });
  filter.init("Filter", &hardwareInterface, 
    [this](bool newState){
      if (newState == true) {
        if (this->maintenanceMode.getValue() != 0 || this->feedMode.getValue() != 0 || this->waterSensorAlarm.getAlarmState() > 0 || this->waterValve.getStateBool() !=  true) {
          return false;
        }
      }
      return true;
    }, [this](Device** devices, int numDevices) {
      this->aqWebServerInterface->deviceStateUpdate(devices, numDevices);
  });
  heater.init("Heater", &hardwareInterface,
    [this](bool newState){
      if (newState == true) {
        return this->filter.getStateBool();
      } else {
        return true;
      }
    },
    [this](Device** devices, int numDevices) {
      this->aqWebServerInterface->deviceStateUpdate(devices, numDevices);
  });
  lights.init("Lights", &hardwareInterface, 
    [this](bool newState){
      return true;
    }, [this](Device** devices, int numDevices) {
      this->aqWebServerInterface->deviceStateUpdate(devices, numDevices);
  });
  co2.init("CO2", &hardwareInterface,
    [this](bool newState){
      return true;
    }, [this](Device** devices, int numDevices) {
      this->aqWebServerInterface->deviceStateUpdate(devices, numDevices);
  });
  airPump.init("Air Pump", &hardwareInterface,
    [this](bool newState){
      return true;
    }, [this](Device** devices, int numDevices) {
      this->aqWebServerInterface->deviceStateUpdate(devices, numDevices);
  });
  co2.attachConnectedDevice(&airPump);
  airPump.attachConnectedDevice(&co2);
  //***************** Init Sensors *****************
  aqTemperature.init("Aquarium Temperature", &hardwareInterface, [this](Sensor* sensor) {
    float valAsFloat = sensor->getValue().toFloat();
    if (valAsFloat < (this->thermostat.getValue() - 0.5)) {
      heater.setStateOn();
    }
    else if (valAsFloat > (this->thermostat.getValue() + 0.5)) {
      heater.setStateOff();
    }
    if (sensor->prevValue != sensor->value) {
      this->aqWebServerInterface->sensorReadingUpdate(sensor);
    } else {
      #ifdef useSerial
        Serial.println("Temperature did not change. Will not update clients.");
      #endif
    }
  });
  tds.init("TDS", &hardwareInterface, &aqTemperature, [this](Sensor* sensor) {
    if (sensor->prevValue != sensor->value) {
      this->aqWebServerInterface->sensorReadingUpdate(sensor);
    }
  });
  waterSensor.init("Water Sensor");
//***************** Init General Settings *****************
  thermostat.init("Thermostat", "TSTAT", [this](GeneralSetting* setting) {
    this->aqTemperature.readSensor();
    this->aqWebServerInterface->settingUpdate(setting);
  });

  maintenanceMode.init("Maintenance Mode", "MT_MD", [this](GeneralSetting* setting) {
    if (setting->getValue() == 1) {
      this->filter.setStateOff();
      this->heater.setStateOff();
    } else {
      this->filter.setStateOn();
      this->aqTemperature.readSensor();
    }
    this->aqWebServerInterface->settingUpdate(setting);
  });
  feedMode.init("Feed Mode", "FD_MD", [this](GeneralSetting* setting) {
    if (setting->getValue() == 1) {
      this->feedModeOffDelay++;
      this->filter.setStateOff();
      this->heater.setStateOff();
      TaskHandle_t xHandle = NULL;
      xTaskCreate([](void* pvParameters) {
        AqController* aqController= (AqController*) pvParameters;
        //Feed Mode delay before turning filter and heater (if necessary) back on.
        //600000 ms for 10 minutes
        const TickType_t xDelay = 600000 / portTICK_PERIOD_MS;
        vTaskDelay(xDelay);
        aqController->feedModeOffDelay--;
        if (aqController->feedModeOffDelay == 0) {
          aqController->feedMode.setValue(0);
        }
        Serial.printf("FEED_MD high water mark %d\n", uxTaskGetStackHighWaterMark(NULL));
        vTaskDelete(NULL);
      },"FEED_MD", 2500, (void *) this, tskIDLE_PRIORITY, &xHandle);
    } else {
      this->filter.setStateOn();
      this->aqTemperature.readSensor();
    }
    this->aqWebServerInterface->settingUpdate(setting);
  });
  
//***************** Init Alarms *****************
  waterSensorAlarm.init("Water Sensor Alarm", "WS_ALM", [this](Alarm* alarm) {
    if (alarm->getAlarmState() > 0) {
      this->heater.setStateOff(true);
      this->filter.setStateOff(true);
      this->airPump.setStateOn(true);
      //const TickType_t xDelay = 100 / portTICK_PERIOD_MS;
      //vTaskDelay(xDelay);
      this->waterValve.setStateOff(true);
    } else {
      this->filter.setStateOn(true);
      this->aqTemperature.readSensor();
      //const TickType_t xDelay = 100 / portTICK_PERIOD_MS;
      //vTaskDelay(xDelay);
      this->waterValve.setStateOn(true);
    }
    this->aqWebServerInterface->alarmUpdate(alarm);
  });
  tasks[0] = new ScheduledTask("Lights On", "Lt_On", SCHEDULED_DEVICE_TASK, [this]() {
    lights.setStateOn();
  });
  tasks[0]->attachConnectedTask("Lights Off", "Lt_Off", [this]() {
    lights.setStateOff();
  });
  tasks[1] = new ScheduledTask("CO2 On", "CO_On", SCHEDULED_DEVICE_TASK, [this]() {
    co2.setStateOn();
  });
  tasks[1]->attachConnectedTask("CO2 Off", "CO_Off", [this]() {
    co2.setStateOff();
  });
  tasks[2] = new ScheduledTask("Air Pump On", "Ar_On", SCHEDULED_DEVICE_TASK, [this]() {
    airPump.setStateOn();
  });
  tasks[2]->attachConnectedTask("Air Pump Off", "Ar_Off", [this]() {
    airPump.setStateOff();
  });
  tasks[3] = new TimedTask ("Read Aquarium Temp", "Rd_Aq_Tmp", TIMED_TASK, [this](){
    aqTemperature.readSensor();
  });
  tasks[4] = new TimedTask ("Read TDS", "Rd_Tds", TIMED_TASK, [this](){
    tds.readSensor();
  });
  aqTemperature.readSensor();
  tds.readSensor();
}

Task* AqController::getTaskByName(String name) {
  //if (tasks == NULL) {
  //  return NULL;
  //}  
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
void AqController::determineTaskRunTimes() {
  for (int i = 0; tasks[i] != NULL; i++) {
    tasks[i]->determineNextRunTime();
    if (tasks[i]->hasConnectedTask()) {
      tasks[i]->connectedTask->determineNextRunTime();
    }
  }
}
void AqController::initSchedDeviceTasks() {
  for (int i = 0; tasks[i] != NULL; i++) {
    if (tasks[i]->taskType == SCHEDULED_DEVICE_TASK) {
      tasks[i]->initTaskState();
    }
  }
}
void AqController::setNextTaskWithEvent() {
  //if (tasks == NULL) {
  //  return;
  //}
  Task* nextTaskWithEventLocal = NULL;
  for (int i = 0; tasks[i] != NULL; i++) {
    //Serial.printf("Checking tasks[%d]. Task name: %s\n", i, tasks[i]->getName().c_str());
    if (tasks[i]->getEnabled() == false) {
      //Serial.printf("tasks[%d]. Task name: %s is disabled. Continuing.\n", i, tasks[i]->getName().c_str());
      continue;
    }
    if (nextTaskWithEventLocal == NULL) {
      nextTaskWithEventLocal = tasks[i];
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
  this->nextTaskWithEvent = nextTaskWithEventLocal;
}
void AqController::scheduleNextTask() {
  setNextTaskWithEvent();
  if (nextTaskWithEvent != NULL) { 
    unsigned long currentLocalEpoch = rtc.getLocalEpoch();
    timerRestart(taskTimer);

    if (nextTaskWithEvent->nextRunTime <= currentLocalEpoch) {
      timerAlarm(taskTimer, 2000, false, 0);
    }
    else {
      unsigned long secondsUntilNextEvent = nextTaskWithEvent->nextRunTime - currentLocalEpoch;

      timerAlarm(taskTimer, secondsUntilNextEvent * 2000, false, 0);
    }
      
  } else {
    timerStop(taskTimer);
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
Alarm* AqController::getAlarmByName(String alarmName) {
  for (int i = 0; i < sizeof(alarms) / sizeof(Alarm*); i++) {
    if (alarms[i]->name == alarmName) {
      return alarms[i];
    }
  }
  return NULL;
}
GeneralSetting* AqController::getSettingByName(String settingName) {
  for (int i = 0; i < sizeof(settings) / sizeof(GeneralSetting*); i++) {
    if (settings[i]->name == settingName) {
      return settings[i];
    }
  }
  return NULL;
}
void AqController::setAqWebServerInterface(AqWebServerInterface* aqWebServerInterface) {
  this->aqWebServerInterface = aqWebServerInterface;
}

