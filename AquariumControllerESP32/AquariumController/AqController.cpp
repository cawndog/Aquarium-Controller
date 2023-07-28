#include "AqController.h"

void AqController::init() {
    //Setup real time clock
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  while (!getLocalTime(&timeinfo)){
    //Wait until the local time is determined.
  }
  AqController::rtc.setTimeStruct(AqController::timeinfo);
  savedState.begin("aqController", false);
  hardwareInterface.init(&savedState);
  Heater.init("Heater", &hardwareInterface);
  Lights.init("Lights", &hardwareInterface);
  CO2.init("CO2", &hardwareInterface);
  AirPump.init("Air Pump", &hardwareInterface);
  Filter.init("Filter", &hardwareInterface);
  CO2.attachConnectedDevice(&AirPump);
  AirPump.attachConnectedDevice(&CO2);
  aqTemperature.init("Aquarium Temperature", &hardwareInterface);
  tds.init("TDS", &hardwareInterface, &aqTemperature);
  for (int i = 0; i < NUM_TASKS; i++)
    tasks[i] = NULL;
  tasks[0] = new ScheduledTask("Lights On", TDEVICE_ON, &savedState, &rtc, NULL, &Lights);
  tasks[0]->connectedTask = new ScheduledTask("Lights Off", TDEVICE_OFF, &savedState, &rtc, &Lights);
  
}

