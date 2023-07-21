#include "HardwareInterface.h"


HardwareInterface::HardwareInterface() {

}
HardwareInterface::~HardwareInterface() {
  delete this->oneWire;
  delete this->tempSensors;
}

void HardwareInterface::init(Preferences* savedState) {
  this->switch1.init("1", savedState);
  this->switch2.init("2", savedState);
  this->switch3.init("3", savedState);
  this->switch4.init("4", savedState);

  this->oneWire = new OneWire(TEMP_SENSOR_PIN);
  this->tempSensors = new DallasTemperature(this->oneWire);
  tempSensors->begin();
  pinMode(TDS_SENSOR_PIN,INPUT);

}
uint8_t HardwareInterface::initDeviceState(String deviceName) {
  //uint8_t returnVal;
  if (deviceName == "Heater") {
    return (switch3.getSwitchState() == ON) ? 1 : 0;
  }
  if (deviceName == "Lights") {
    return (switch2.getSwitchState() == ON) ? 1 : 0;
  }
  if (deviceName == "CO2") {
    return (switch1.getSwitchState() == AUXON) ? 1 : 0;
  }
  if (deviceName == "Air Pump") {
    return (switch1.getSwitchState() == ON) ? 1 : 0;
  }
  if (deviceName == "Filter") {
    return (switch4.getSwitchState() == ON) ? 1 : 0;
  }
  return 0;

}
void HardwareInterface::powerControl(String deviceName, uint8_t state) {
  if (deviceName == "Heater") {
    state == 1 ? switch3.powerControl(ON) : switch3.powerControl(OFF);
  }
  else if (deviceName == "Lights") {
    state == 1 ? switch2.powerControl(ON) : switch2.powerControl(OFF);
  }
  else if (deviceName == "CO2") {
    state == 1 ? switch1.powerControl(AUXON) : switch1.powerControl(OFF);
  }
  else if (deviceName == "Air Pump") {
    state == 1 ? switch1.powerControl(ON) : switch1.powerControl(OFF);
  }
  else if (deviceName == "Filter") {
    state == 1 ? switch4.powerControl(ON) : switch4.powerControl(OFF);
  }
 
}

float HardwareInterface::readTdsSensor(float temperature) {
  temperature = (temperature - 32.0)*(5.0/9.0); //convert temp to Celsius
    #ifdef useSerial
    Serial.print("Reading TDS. Temp passed in: ");
    Serial.println(temperature);
    SerialBT.print("Reading TDS. Temp passed in: ");
    SerialBT.println(temperature);
  #endif
  int analogBuffer[SCOUNT];     // store the analog value in the array, read from ADC
  //int analogBufferTemp[SCOUNT];
  int analogBufferIndex = 0;
  int copyIndex = 0;
  float averageVoltage = 0;
  for(int i = 0; i < SCOUNT; i++) {
    //every 40 getMillis()econds,read the analog value from the ADC
    analogBuffer[analogBufferIndex] = analogRead(TDS_SENSOR_PIN);    //read the analog value and store into the buffer
    analogBufferIndex++;
    if(analogBufferIndex == SCOUNT){ 
      analogBufferIndex = 0;
    }
    delay(40);
  }    
  
  /*for(copyIndex=0; copyIndex<SCOUNT; copyIndex++)
    analogBufferTemp[copyIndex] = analogBuffer[copyIndex];*/
    
  // read the analog value more stable by the median filtering algorithm, and convert to voltage value
  averageVoltage = getMedianNum(analogBuffer,SCOUNT) * (float)VREF / 4096.0;
  //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0)); 
  float compensationCoefficient = 1.0+0.02*(temperature-25.0);
  //temperature compensation
  float compensationVoltage=averageVoltage/compensationCoefficient;
  //convert voltage value to tds value
  this->tdsSensor.value = 

  return ((133.42*compensationVoltage*compensationVoltage*compensationVoltage - 255.86*compensationVoltage*compensationVoltage + 857.39*compensationVoltage)*0.5); 
}
// median filtering algorithm
int HardwareInterface::getMedianNum(int bArray[], int iFilterLen){
  int bTab[iFilterLen];
  for (uint8_t i = 0; i<iFilterLen; i++)
  bTab[i] = bArray[i];
  int i, j, bTemp;
  for (j = 0; j < iFilterLen - 1; j++) {
    for (i = 0; i < iFilterLen - j - 1; i++) {
      if (bTab[i] > bTab[i + 1]) {
        bTemp = bTab[i];
        bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
      }
    }
  }
  if ((iFilterLen & 1) > 0){
    bTemp = bTab[(iFilterLen - 1) / 2];
  }
  else {
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
  }
  return bTemp;
}
float HardwareInterface::readAquariumTemperatureSensor() {
  this->tempSensors->requestTemperaturesByIndex(0);
  return this->tempSensors->getTempFByIndex(0);
}

