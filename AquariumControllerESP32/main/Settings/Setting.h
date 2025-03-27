#ifndef SETTING_H
#define SETTING_H
#include <Preferences.h>
#include <ESP32Time.h>

extern Preferences savedState;
extern ESP32Time rtc;
class GeneralSetting {
  public:
    typedef std::function<void(GeneralSetting* setting)> SetValueFunction;
    String name;
    String shortName;
    int8_t value;
    unsigned long lastUpdated;
    SetValueFunction setValueFunction;
    GeneralSetting();
    void init(String name, String shortName, SetValueFunction setValueFunction);
    String getName();
    int8_t getValue();
    unsigned long getLastUpdated();
    void setValue(int8_t newVal);
};

#endif