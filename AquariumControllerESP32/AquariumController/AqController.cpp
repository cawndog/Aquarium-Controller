#include "AqController.h"

void AqController::init() {
    //Setup real time clock
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  while (!getLocalTime(&timeinfo)){
    //Wait until the local time is determined.
  }
  rtc.setTimeStruct(timeinfo);
  savedState.begin("aqController", false);
  hardwareInterface.init(&savedState);
  Heater.init(&hardwareInterface)
  Lights.init(&hardwareInterface);
  CO2.init(&hardwareInterface);
  AirPump.init(&hardwareInterface);
  Filter.init(&hardwareInterface);
  CO2.attachConnectedDevice(&AirPump);
  AirPump.attachConnectedDevice(&CO2);
  aqTemperature.init(&hardwareInterface);
  tds.init(&hardwareInterface, &aqTemperature);
  for (int i = 0; i < NUM_TASKS; i++)
    tasks[i] = NULL;

  tasks[0] = new ScheduledTask("Lights On", TDEVICE_ON, &Lights, &savedState, &rtc);
  tasks[0]->connectedTask = new ScheduledTask("Lights Off", TDEVICE_OFF, &Lights, &savedState, &rtc);
}

