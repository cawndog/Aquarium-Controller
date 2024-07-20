#ifndef SETTING_H
#define SETTING_H
#include <Preferences.h>

extern Preferences savedState;

class GeneralSetting {
  public:
    typedef std::function<void(GeneralSetting* setting)> SetValueFunction;
    String name;
    String shortName;
    int8_t value;
    SetValueFunction setValueFunction;
    GeneralSetting();
    void init(String name, String shortName, SetValueFunction setValueFunction);
    String getName();
    int8_t getValue();
    void setValue(int8_t newVal);
};

#endif