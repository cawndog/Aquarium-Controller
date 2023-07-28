#ifndef TASK_H
#define TASK_H
#include <ESP32Time.h>
#include <Arduino.h>
#include "Sensor.h"
#include "Device.h"
#include <Preferences.h>

enum TaskType {
  TDEVICE_ON,
  TDEVICE_OFF,
  SENSOR_READ,
  MAINTENANCE_OFF
};
struct TaskSettings {
  bool disabled;
  unsigned long time;
};
class Task {
  public: 
    String name;
    unsigned long nextRunTime; //Next run time in Epoch Time
    TaskType taskType;
    Preferences* savedState;
    ESP32Time* rtc;
    TaskSettings settings;
    bool privateTask = false; //task and its settings will not be passed to clients for viewing/editing.
    Task* connectedTask = NULL;
    //Task(String name);
    //virtual ~Task() {};
    virtual void doTask() = 0;
    virtual void determineNextRunTime() = 0;
    virtual void updateSettings(bool disabled, unsigned long time) = 0;
    virtual void initTaskState() = 0;
    void setPrivate();
    String getName();
    bool getDisabled(); 
    unsigned long getTime();

    
};

class ScheduledTask : public Task {
  public: 
    Device *device;
    void (*Taskfunc);
    /*struct STSettings {
      bool disabled;
      uint16_t runTime;
    };
    STSettings settings;*/
    ScheduledTask(String name, TaskType taskType, Preferences* savedState, ESP32Time* rtc, void (*Taskfunc) = NULL, Device* device = NULL);
    void doTask();
    void determineNextRunTime();
    void updateSettings(bool disabled, unsigned long time);
    void initTaskState(); //For Scheduled Timer (On/Off) Tasks. Inits timer state. 
};
class TimedTask : public Task {
  public: 
    Device* device;
    Sensor* sensor;
    /*struct TTSettings {
      bool disabled;
      unsigned long runTimeInterval;
    };

    TTSettings settings;*/
    TimedTask(String name, TaskType taskType, Preferences* savedState, ESP32Time* rtc, void (*Taskfunc) = NULL, Device* device = NULL, Sensor* sensor = NULL);
    void doTask();
    void determineNextRunTime();
    void updateSettings(bool disabled, unsigned long time);
};




#endif