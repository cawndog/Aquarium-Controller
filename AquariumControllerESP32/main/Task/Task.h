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
    AqTaskFunction f; //Holds the function that this task executes.
    unsigned long nextRunTime; //Next run time in Epoch Time
    TaskType taskType;
    bool enabled; //allows for enablement/disablement of task
    unsigned long time; //This variable is treated differently depending on the derived class that is using it. Holds task scheduling data.
    Task* connectedTask = NULL;
    TaskHandle_t xHandle = NULL; //Task handle of the freeRTOS xTask that runs this task's function, f. 

    String getName();
    bool getEnabled(); 
    void scheduleTaskToRun(); //Sets up a freeRTOS xTask that blocks until this task's next run time. Once unblocked, it calls doTask().
    void unscheduleTask();
    unsigned long getTime(); //returns task scheduling data, the variable named time.
    String taskTypeToString();
    void sleepUntil(TickType_t *prevWakeTime,  unsigned int sec );

    virtual void doTask() = 0;
    virtual void determineNextRunTime() = 0;
    virtual void updateSettings(bool enabled, unsigned long time) = 0;
    virtual void initTaskState() = 0;
    virtual void runF() = 0;
    virtual void attachConnectedTask(String name, String shortName, AqTaskFunction f = [](){}) = 0;
    virtual bool hasConnectedTask() = 0;


};

class ScheduledTask : public Task {
  //unsigned long time is the time, in seconds past 12:00AM, that this task will be scheduled to run.
  public: 
    //AqTaskFunction f;
    //void (*Taskfunc);
    ScheduledTask(String name, String shortName, TaskType taskType, AqTaskFunction f = [](){});
    void doTask();
    void runF();
    void attachConnectedTask(String name, String shortName, AqTaskFunction f = [](){});
    bool hasConnectedTask();
    void determineNextRunTime();
    void updateSettings(bool enabled, unsigned long time);
    void initTaskState(); //For ScheduledTask with a connectedTask (On/Off timer tasks). Inits timer state. 
};
class TimedTask : public Task {
  //unsigned long time is the time interval, in seconds, that this task will be scheduled to run.
  public: 
    //AqTaskFunction f;
    TimedTask(String name, String shortName, TaskType taskType, AqTaskFunction f = {});
    void doTask();
    void runF();
    void attachConnectedTask(String name, String shortName, AqTaskFunction f = [](){});
    bool hasConnectedTask();
    void determineNextRunTime();
    void updateSettings(bool enabled, unsigned long time);
    void initTaskState();
};



#endif