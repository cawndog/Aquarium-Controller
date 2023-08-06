#include "AqController.h"

void AqController::init(AqWebServerInterface* aqWebServerInterface) {
    //Setup real time clock
  this->aqWebServerInterface = aqWebServerInterface;
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  while (!getLocalTime(&timeinfo)){
    //Wait until the local time is determined.
  }
  AqController::rtc.setTimeStruct(AqController::timeinfo);
  savedState.begin("aqController", false);
  hardwareInterface.init(&savedState);
  heater.init("Heater", &hardwareInterface);
  lights.init("Lights", &hardwareInterface);
  co2.init("CO2", &hardwareInterface);
  airPump.init("Air Pump", &hardwareInterface);
  filter.init("Filter", &hardwareInterface);
  co2.attachConnectedDevice(&airPump);
  airPump.attachConnectedDevice(&co2);
  aqTemperature.init("Aquarium Temperature", &hardwareInterface);
  tds.init("TDS", &hardwareInterface, &aqTemperature);
  for (int i = 0; i < NUM_TASKS; i++)
    tasks[i] = NULL;
  tasks[0] = new ScheduledTask("Lights On", TDEVICE_ON, &savedState, &rtc, NULL, &lights);
  tasks[0]->connectedTask = new ScheduledTask("Lights Off", TDEVICE_OFF, &savedState, &rtc, NULL, &lights);
  tasks[1] = new ScheduledTask("CO2 On", TDEVICE_ON, &savedState, &rtc, NULL, &co2);
  tasks[1]->connectedTask = new ScheduledTask("CO2 Off", TDEVICE_OFF, &savedState, &rtc, NULL, &co2);
  tasks[2] = new ScheduledTask("Air Pump On", TDEVICE_ON, &savedState, &rtc, NULL, &airPump);
  tasks[2]->connectedTask = new ScheduledTask("Air Pump Off", TDEVICE_OFF, &savedState, &rtc, NULL, &airPump);
  tasks[3] = new TimedTask ("Read Aquarium Temp", SENSOR_READ, &savedState, &rtc, NULL, NULL, &aqTemperature);
  tasks[4] = new TimedTask ("Read TDS", SENSOR_READ, &savedState, &rtc, NULL, NULL, &tds);

  for (int i = 0; i < 3; i++) {
    tasks[i]->initTaskState();
  }
}

