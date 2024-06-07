#ifndef AQ_WEB_SERVER_INTERFACE_H
#define AQ_WEB_SERVER_INTERFACE_H
class Device;
class Sensor;
class AqWebServerInterface {
  public:
    AqWebServerInterface() {};
    virtual void deviceStateUpdate(Device** devices, int numDevices) = 0; //Sends update to Web Socket clients when a device state changes.
    virtual void sensorReadingUpdate(Sensor* sensor) = 0; //Sends update to Web Socket clients when a sensor value changes
    virtual void updateDynamicIP() = 0;
    ~AqWebServerInterface() {};
};


#endif