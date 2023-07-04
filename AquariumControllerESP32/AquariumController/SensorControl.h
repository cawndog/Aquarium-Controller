#ifndef SENSOR_CONTROL_H
#define SENSOR_CONTROL_H

#include <ESP32Time.h>
#include "AquariumController.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include "Sensor.h"
#include "string.h"

#define VREF 3.3              // analog reference voltage(Volt) of the ADC
#define SCOUNT  30            // sum of sample point

#ifdef useSerial
  #include <BluetoothSerial.h>
  extern BluetoothSerial SerialBT;
#endif
class SensorControl {
    private:
      //float tdsVal;
      uint8_t aquariumThermostat;
      //float aquariumTemp;
      OneWire *oneWire;
      DallasTemperature *tempSensors;
      int getMedianNum(int bArray[], int iFilterLen); //For reading TDS value
    public:
      Sensor aquariumTemp;
      Sensor tdsSensor; 

      SensorControl();
      void init();
      void readTds(float temperature);
      void readAquariumTemp(); 
      int getTdsVal();        //returns TDS value as an int
      float getTdsValFloat();     //returns TDS value as a float
      float getAquariumTemp();    //returns aquarium temp in F. //updated
      float getAquariumTempC();   //returns aquarium temp in C
      float round(float var);     //Round float to 1 decimal place
      uint8_t getAquariumThermostat();
      void setAquariumThermostat(uint8_t temp);
      bool valuesUpdated();
};
#endif