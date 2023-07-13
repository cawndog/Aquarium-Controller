#include "HardwareInterface.h"


HardwareInterface::HardwareInterface() {

}
HardwareInterface::~HardwareInterface() {
  delete this->oneWire;
  delete this->tempSensors;
}

void HardwareInterface::init(SavedStateController* savedStateController) {
  this->switch1.init(&(savedStateController->switch1State));
  this->switch2.init(&(savedStateController->switch2State));
  this->switch3.init(&(savedStateController->switch3State));
  this->switch4.init(&(savedStateController->switch4State));

  this->oneWire = new OneWire(TEMP_SENSOR_PIN);
  this->tempSensors = new DallasTemperature(this->oneWire);
  tempSensors->begin();
  pinMode(TDS_SENSOR_PIN,INPUT);

}
DeviceState HardwareInterface::initDeviceState(Device* device) {
  switch(device->name) {
    case "Heater": {
      switch3.getSwitchState() = ON ? return ON : return OFF;
    }
    case "Lights": {
      switch2.getSwitchState() = ON ? return ON : return OFF;
    }
    case "CO2": {
      switch1.getSwitchState() = AUXON ? return ON : return OFF;
      return;
    }
    case "Air Pump:" {
      switch1.getSwitchState() = ON ? return ON : return OFF;
      return;
    }
    case "Filter": {
      switch4.getSwitchState() = ON ? return ON : return OFF;
      return;
    }
    case default: {
      return;
    } 
  };
}
SwitchState HardwareInterface::powerControl(Device* device) {
  switch(device->name) {
    case "Heater": {
      device->state = ON ? switch3.powerControl(ON) : switch3.powerControl(OFF);
      return;
    }
    case "Lights": {
      device->state = ON ? switch2.powerControl(ON) : switch2.powerControl(OFF);
      return;
    }
    case "CO2": {
      device->state = ON ? switch1.powerControl(AUXON) : switch1.powerControl(OFF);
      return;
    }
    case "Air Pump:" {
      device->state = ON ? switch1.powerControl(ON) : switch1.powerControl(OFF);
      return;
    }
    case "Filter": {
      device->state = ON ? switch4.powerControl(ON) : switch4.powerControl(OFF);
      return;
    }
    case default: {
      return;
    } 
  };
  
}

float HardwareInterface::readTdsSensor(float temperature) {
  temperature = (temperature - 32.0)*(5.0/9.0); //convert temp to Celsius
    #ifdef useSerial
    Serial.print("Reading TDS. Temp passed in: ");
    Serial.println(temperature);
    SerialBT.print("Reading TDS. Temp passed in: ");
    SerialBT.println(temperature);
  #endif
  int previousTDSval = this->getTdsVal();
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
  this->tdsSensor.value = ((133.42*compensationVoltage*compensationVoltage*compensationVoltage - 255.86*compensationVoltage*compensationVoltage + 857.39*compensationVoltage)*0.5);
  if (previousTDSval != this->getTdsVal()) {
    this->tdsSensor.valueUpdated = true;
  }
  return; 
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

