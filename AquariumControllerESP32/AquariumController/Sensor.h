#ifndef SENSOR_H
#define SENSOR_H
#include <string>

class Sensor {
  public:
    String name;
    float value;
    bool valueUpdated;

    Sensor(String name) {
      this->name = name;
      this->value = 0.0;
      this->valueUpdated = false;
    }
}
#endif