#ifndef TASK_H
#define TASK_H
#include <ESP32Time.h>
#include <Arduino.h>
#include <Preferences.h>
extern ESP32Time rtc;
extern Preferences savedState;
enum TaskType {
  SCHEDULED_TASK,
  SCHEDULED_DEVICE_TASK,
  TIMED_TASK,
};
typedef std::function<void()> AqTaskFunction;
class Task {
  public: 
    String name;
    String shortName;
    unsigned long nextRunTime; //Next run time in Epoch Time
    TaskType taskType;
    bool disabled;
    unsigned long time;
    Task* connectedTask = NULL;

    virtual void doTask() = 0;
    virtual void determineNextRunTime() = 0;
    virtual void updateSettings(bool disabled, unsigned long time) = 0;
    //virtual void initTaskState() = 0;
    virtual void runF() = 0;
    virtual void attachConnectedTask(String name, String shortName, AqTaskFunction f = [](){}) = 0;
    virtual bool hasConnectedTask() = 0;
    String taskTypeToString();
    String getName();
    bool getDisabled(); 
    //returns the scheduled run time of this task (in seconds). For TimedTask, this would be the time interval that the task will run. 
    unsigned long getTime(); 

    
};

class ScheduledTask : public Task {
  public: 
    AqTaskFunction f;
    //void (*Taskfunc);
    ScheduledTask(String name, String shortName, TaskType taskType, AqTaskFunction f = [](){});
    void doTask();
    void runF();
    void attachConnectedTask(String name, String shortName, AqTaskFunction f = [](){});
    bool hasConnectedTask();
    void determineNextRunTime();
    void updateSettings(bool disabled, unsigned long time);
    void initTaskState(); //For ScheduledTask with a connectedTask (On/Off timer tasks). Inits timer state. 
};
class TimedTask : public Task {
  public: 
    AqTaskFunction f;
    TimedTask(String name, String shortName, TaskType taskType, AqTaskFunction f = {});
    void doTask();
    void runF();
    void attachConnectedTask(String name, String shortName, AqTaskFunction f = [](){});
    bool hasConnectedTask();
    void determineNextRunTime();
    void updateSettings(bool disabled, unsigned long time);
    //void initTaskState();
};



#endif