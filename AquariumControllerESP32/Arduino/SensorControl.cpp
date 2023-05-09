#include "SensorControl.h"


SensorControl::SensorControl() {
  this->tdsVal = 0.00;
  this->aquariumTemp = 0.00;
}
void SensorControl::init() {
  this->oneWire = new OneWire(TEMP_SENSOR_PIN);
  this->tempSensors = new DallasTemperature(this->oneWire);
  tempSensors->begin();
  pinMode(TDS_SENSOR_PIN,INPUT);
  this->readAquariumTemp();
  this->readTds(getAquariumTempC());
  this->aquariumThermostat = EEPROM.read(aquariumThermostatLoc);
  return;
}
float SensorControl::getAquariumTemp() {
  return this->aquariumTemp;
}
float SensorControl::getAquariumTempC() {
  float temp = this->aquariumTemp;
  temp = (temp - 32.0)*(5.0/9.0);  
  return temp;
}
float SensorControl::getTdsValFloat() {
  return this->tdsVal;
}
int SensorControl::getTdsVal() {
  return int(this->tdsVal);
}
void SensorControl::readAquariumTemp() {
  #ifdef useSerial
    Serial.println("Reading temperature.");
    SerialBT.println("Reading temperature.");
  #endif
  this->tempSensors->requestTemperaturesByIndex(0);
  float Temp = this->tempSensors->getTempFByIndex(0);
  this->aquariumTemp = this->round(Temp);
  return;
}
float SensorControl::round (float var) {
  float val = floor(var * 10.0) / 10.0;
  return val;
}
// median filtering algorithm
int SensorControl::getMedianNum(int bArray[], int iFilterLen){
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
uint8_t SensorControl::getAquariumThermostat() {
  return this->aquariumThermostat;
}
void SensorControl::setAquariumThermostat(uint8_t temp) {
  this->aquariumThermostat = temp;
  EEPROM.write(aquariumThermostatLoc, temp);
}
void SensorControl::readTds(float temperature){
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
  this->tdsVal = ((133.42*compensationVoltage*compensationVoltage*compensationVoltage - 255.86*compensationVoltage*compensationVoltage + 857.39*compensationVoltage)*0.5);
  return; 
}
