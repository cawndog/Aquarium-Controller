#ifndef AQ_WEB_SERVER_INTERFACE_H
#define AQ_WEB_SERVER_INTERFACE_H
class Device;
class Sensor;
class AqWebServerInterface {
  public:
    AqWebServerInterface() {}
    virtual void deviceStateUpdate(Device** devices, int numDevices) = 0;
    virtual void sensorReadingUpdate(Sensor* sensor) = 0;
    virtual void updateDynamicIP();
};
#endif