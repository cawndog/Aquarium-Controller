#ifndef ALARM_H
#define ALARM_H
#include <Preferences.h>

extern Preferences savedState;

class Alarm {
  public:
    typedef std::function<void(Alarm* alarm)> SetAlarmStateFunction;
    String name; //Name of alarm
    String shortName;
    unsigned long alarmState; //State of alarm. 0 = OK. 1 = Alarm Active
    SetAlarmStateFunction setAlarmStateFunction;
    Alarm();
    void init(String name, String shortName, SetAlarmStateFunction setAlarmStateFunction);
    String getName();
    unsigned long getAlarmState(); //Returns state of alarm. 0 = OK. 1 = Alarm Active
    void setAlarmState(unsigned long newState); //Reset alarm state back to 0
};

#endif