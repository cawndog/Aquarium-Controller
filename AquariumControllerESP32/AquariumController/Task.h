#ifndef TASK_H
#define TASK_H
#include <ESP32Time.h>
#include <Arduino.h>
#include "Sensor.h"
#include "Device.h"
#include <Preferences.h>

enum TaskType {
  SCHEDULED_TASK,
  SCHEDULED_DEVICE_TASK,
  TIMED_TASK,
};
struct TaskSettings {
  bool disabled;
  unsigned long time;
};
typedef std::function<void()> AqTaskFunction;
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
    String taskTypeToString();
    String getName();
    bool getDisabled(); 
    //returns the scheduled run time of this task (in seconds). For TimedTask, this would be the time interval that the task will run. 
    unsigned long getTime(); 

    
};

class ScheduledTask : public Task {
  public: 
    Device *device;
    AqTaskFunction f;
    void (*Taskfunc);
    /*struct STSettings {
      bool disabled;
      uint16_t runTime;
    };
    STSettings settings;*/
    ScheduledTask(String name, TaskType taskType, Preferences* savedState, ESP32Time* rtc, AqTaskFunction f = {});
    void doTask();
    void determineNextRunTime();
    void updateSettings(bool disabled, unsigned long time);
    void initTaskState(); //For ScheduledTask with a connectedTask (On/Off timer tasks). Inits timer state. 
};
class TimedTask : public Task {
  public: 
    Device* device;
    Sensor* sensor;
    AqTaskFunction f;
    /*struct TTSettings {
      bool disabled;
      unsigned long runTimeInterval;
    };

    TTSettings settings;*/
    TimedTask(String name, TaskType taskType, Preferences* savedState, ESP32Time* rtc, AqTaskFunction f = {});
    void doTask();
    void determineNextRunTime();
    void updateSettings(bool disabled, unsigned long time);
    void initTaskState();
};




#endif