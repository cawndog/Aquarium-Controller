#include "HardwareInterface.h"

HardwareInterface::HardwareInterface() {

}
HardwareInterface::~HardwareInterface() {
  delete this->oneWire;
  delete this->tempSensors;
}

void HardwareInterface::init() {
  pinMode(WATER_VALVE_Y_PIN, OUTPUT);
  pinMode(WATER_VALVE_R_PIN, OUTPUT);
  pinMode(TDS_SENSOR_PIN,INPUT);
  this->switch1.init("1");
  this->switch2.init("2");
  this->switch3.init("3");
  this->switch4.init("4");

  this->oneWire = new OneWire(TEMP_SENSOR_PIN);
  this->tempSensors = new DallasTemperature(this->oneWire);
  tempSensors->begin();
  tempSensors->setWaitForConversion(true);
  
  waterValveSemaphore = xSemaphoreCreateBinary();
  xSemaphoreGive(waterValveSemaphore);
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
    /*if (savedState.getUChar(deviceName.c_str(), 0) == 1) {
      digitalWrite(CO2_PIN, HIGH);
      return 1;
    } else {
      digitalWrite(CO2_PIN, LOW);
      return 0;
    }*/
  }
  if (deviceName == "Air Pump") {
    return (switch1.getSwitchState() == ON) ? 1 : 0;
  }
  if (deviceName == "Filter") {
    return (switch4.getSwitchState() == ON) ? 1 : 0;
  }
  if (deviceName == "Water Valve") {
    uint8_t state = savedState.getUChar(deviceName.c_str(), 0);
    this->powerControl("Water Valve", state);
    return state;
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
    /*if (state == 1) {
      savedState.putUChar(deviceName.c_str(), 1);
      digitalWrite(CO2_PIN, HIGH);
    } else {
      savedState.putUChar(deviceName.c_str(), 0);
      digitalWrite(CO2_PIN, LOW);
    }*/
  }
  else if (deviceName == "Air Pump") {
    state == 1 ? switch1.powerControl(ON) : switch1.powerControl(OFF);
  }
  else if (deviceName == "Filter") {
    state == 1 ? switch4.powerControl(ON) : switch4.powerControl(OFF);
  }
  else if (deviceName == "Water Valve") {
    #ifdef useSerial
      Serial.printf("In PowerControl for Water Valve\n");
    #endif
    if (state == 1) { //turn on
      xSemaphoreTake(waterValveSemaphore, portMAX_DELAY);
        savedState.putUChar(deviceName.c_str(), 1);
        this->waterValvePinsOffDelay++;
        digitalWrite(WATER_VALVE_R_PIN, HIGH);
        digitalWrite(WATER_VALVE_Y_PIN, HIGH);
        TaskHandle_t xHandle = NULL;
        xTaskCreate([](void* pvParameters) {
          HardwareInterface* hardwareInterface = (HardwareInterface*) pvParameters;
          const TickType_t xDelay = 7000 / portTICK_PERIOD_MS;
          vTaskDelay(xDelay);
          xSemaphoreTake(waterValveSemaphore, portMAX_DELAY);
            hardwareInterface->waterValvePinsOffDelay--;
            if (hardwareInterface->waterValvePinsOffDelay == 0) {
              digitalWrite(WATER_VALVE_Y_PIN, LOW);
              digitalWrite(WATER_VALVE_R_PIN, LOW);
            }
          xSemaphoreGive(waterValveSemaphore);
          Serial.printf("WV_On high water mark %d\n", uxTaskGetStackHighWaterMark(NULL));
          vTaskDelete(NULL);
        },"WV_On", 2000, (void *) this, tskIDLE_PRIORITY, &xHandle);
      xSemaphoreGive(waterValveSemaphore);
      //configASSERT(xHandle);
    }
    else { //turn off
      xSemaphoreTake(waterValveSemaphore, portMAX_DELAY);
        savedState.putUChar(deviceName.c_str(), 0);
        this->waterValvePinsOffDelay++;
        digitalWrite(WATER_VALVE_R_PIN, LOW);
        digitalWrite(WATER_VALVE_Y_PIN, HIGH);
        TaskHandle_t xHandle = NULL;
        xTaskCreate([](void* pvParameters) {
          HardwareInterface* hardwareInterface = (HardwareInterface*) pvParameters;
          const TickType_t xDelay = 8500 / portTICK_PERIOD_MS;
          vTaskDelay(xDelay);
          xSemaphoreTake(waterValveSemaphore, portMAX_DELAY);
            hardwareInterface->waterValvePinsOffDelay--;
            if (hardwareInterface->waterValvePinsOffDelay == 0) {
              digitalWrite(WATER_VALVE_Y_PIN, LOW);
              digitalWrite(WATER_VALVE_R_PIN, LOW);
            }
          xSemaphoreGive(waterValveSemaphore);
          Serial.printf("WV_Off high water mark %d\n", uxTaskGetStackHighWaterMark(NULL));
          vTaskDelete(NULL);
        },"WV_Off", 2000, (void *) this, tskIDLE_PRIORITY, &xHandle);
      xSemaphoreGive(waterValveSemaphore);
      //configASSERT(xHandle);
    }
  }
}
float HardwareInterface::readTdsSensor(float temperature) {
  temperature = (temperature - 32.0)*(5.0/9.0); //convert temp to Celsius
  #ifdef useSerial
    Serial.print("Reading TDS. Temp passed in: ");
    Serial.println(temperature);
  #endif
  #ifdef useSerialBT
    SerialBT.print("Reading TDS. Temp passed in: ");
    SerialBT.println(temperature);
  #endif
  int analogBuffer[SCOUNT];     // store the analog value in the array, read from ADC
  int analogBufferIndex = 0;
  //int copyIndex = 0;
  float averageVoltage = 0;
  for(int i = 0; i < SCOUNT; i++) {
    //every 15 milliseconds,read the analog value from the ADC
    analogBuffer[analogBufferIndex] = analogRead(TDS_SENSOR_PIN);    //read the analog value and store into the buffer
    analogBufferIndex++;
    if(analogBufferIndex == SCOUNT){ 
      analogBufferIndex = 0;
    }
    const TickType_t xDelay = 10 / portTICK_PERIOD_MS;
    vTaskDelay(xDelay);
  }
  // read the analog value more stable by the median filtering algorithm, and convert to voltage value
  averageVoltage = getMedianNum(analogBuffer,SCOUNT) * (float)VREF / 4096.0;
  //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0)); 
  float compensationCoefficient = 1.0+0.02*(temperature-25.0);
  //temperature compensation
  float compensationVoltage=averageVoltage/compensationCoefficient;
  //convert voltage value to tds value

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

