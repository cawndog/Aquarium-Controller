#ifndef EVENT_H
#define EVENT_H
#include <String.h>
#include "Sensor.h"
#include "Device.h"
enum TaskType {
  DEVICE_ON
  DEVICE_OFF
  SENSOR_READ
  MAINTENANCE_OFF
};
class Task {
  public: 
    //String name;
    //unsigned long nextScheduledEpochTime;
    String name;
    unsigned long nextRunTime; //Next run time in Epoch Time
    TaskType taskType;
    virtual ~Task() {};
    virtual void doTask() = 0;
    virtual void determineNextRunTime() = 0;
};

class ScheduledTask : public Task {
  Device *device;
  bool disabled;
  void doTask();
  void determineNextRunTime();
};
class TimedTask : public Task {
  Device *device;
  Sensor *sensor;
  bool disabled;
  void doTask();
  void determineNextRunTime();
}




#endif