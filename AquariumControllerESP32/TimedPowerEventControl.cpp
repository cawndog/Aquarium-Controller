#include "TimedPowerEventControl.h"

TimedDevice::TimedDevice() {

}
void TimedDevice::init(String deviceName, uint8_t onHrLoc, uint8_t onMinLoc, uint8_t offHrLoc, int8_t offMinLoc, ESP32Time *rtc, PowerControl *powerControl) {
    this->deviceName = deviceName;
    this->onHr = EEPROM.read(onHrLoc);
    this->onMin = EEPROM.read(onMinLoc);
    this->offHr = EEPROM.read(offHrLoc);
    this->offMin = EEPROM.read(offMinLoc);
    this->onHrSaveLoc = onHrLoc;
    this->onMinSaveLoc = onMinLoc;
    this->offHrSaveLoc = offHrLoc;
    this->offMinSaveLoc = offMinLoc;
    this->initDeviceState(rtc, powerControl);
}
void TimedDevice::updateDeviceState(ESP32Time *rtc, PowerControl *powerControl) {
    unsigned long nextEventTime = rtc->getLocalEpoch();
    int currentHour = rtc->getHour(true);
    int currentMinute = rtc->getMinute();
    int currentSecond = rtc->getSecond();
    unsigned long secondsSinceStartOfDay = ((currentHour*3600)+(currentMinute*60)+currentSecond); //Seconds since 12:00AM
    unsigned long onTimeInSeconds = ((this->onHr*3600)+(this->onMin*60));
    unsigned long offTimeInSeconds = ((this->offHr*3600)+(this->offMin*60));
    #ifdef useSerial
        Serial.print("In updateDeviceState(). Updating state of ");
        Serial.println(this->deviceName);
        Serial.print("Current Epoch time: ");
        Serial.println(nextEventTime);
        SerialBT.print("In updateDeviceState(). Updating state of ");
        SerialBT.println(this->deviceName);
        SerialBT.print("Current Epoch time: ");
        SerialBT.println(nextEventTime);
    #endif
    if (onTimeInSeconds < offTimeInSeconds) { //device turns on and off within the same 00:00-23:59 period
        if (secondsSinceStartOfDay >= onTimeInSeconds && secondsSinceStartOfDay < offTimeInSeconds) {
            this->turnDeviceOn(powerControl);
            //Calculate Epoch time for device's next off time
            nextEventTime += (offTimeInSeconds - secondsSinceStartOfDay);
        }
        else {
            this->turnDeviceOff(powerControl);
            //Calculate Epoch time for device's next on time
            if (secondsSinceStartOfDay < onTimeInSeconds) { //Device has not turned on for the day yet
                nextEventTime += (onTimeInSeconds - secondsSinceStartOfDay); 
            } else { //Device has turned off for the day
                nextEventTime += (onTimeInSeconds + (86400-secondsSinceStartOfDay));
            }
        }
    }
    else if(onTimeInSeconds > offTimeInSeconds) { //Device turns on and off within different 00:00-23:59 periods (ie. device turns on at night and off the next morning)
        if (secondsSinceStartOfDay >= onTimeInSeconds || secondsSinceStartOfDay < offTimeInSeconds) {
            this->turnDeviceOn(powerControl);
            //Calculate Epoch time for device's next off time
            if (secondsSinceStartOfDay >= onTimeInSeconds) { //
                nextEventTime += (offTimeInSeconds + (86400-secondsSinceStartOfDay));
            }
            else {
                nextEventTime += (offTimeInSeconds - secondsSinceStartOfDay);
            }
            
        }
        else {
            this->turnDeviceOff(powerControl);
            //Calculate Epoch time for device's next on time
            nextEventTime += (onTimeInSeconds - secondsSinceStartOfDay);
        }
    }
    else { //onTime and offTime are the same
        this->turnDeviceOff(powerControl);
    }
     #ifdef useSerial
        Serial.print("Next event Epoch time: ");
        Serial.println(nextEventTime);
        Serial.println("Leaving updateDeviceState()");
        SerialBT.print("Next event Epoch time: ");
        SerialBT.println(nextEventTime);
        SerialBT.println("Leaving updateDeviceState()");
    #endif
    this->nextEventEpochTime = nextEventTime;
    return;
}
void TimedDevice::initDeviceState(ESP32Time *rtc, PowerControl *powerControl) {
    unsigned long nextEventTime = rtc->getLocalEpoch();
    int currentHour = rtc->getHour(true);
    int currentMinute = rtc->getMinute();
    int currentSecond = rtc->getSecond();
    unsigned long secondsSinceStartOfDay = ((currentHour*3600)+(currentMinute*60)+currentSecond); //Seconds since 12:00AM
    unsigned long onTimeInSeconds = ((this->onHr*3600)+(this->onMin*60));
    unsigned long offTimeInSeconds = ((this->offHr*3600)+(this->offMin*60));
    #ifdef useSerial
        Serial.print("In updateDeviceState(). Updating state of ");
        Serial.println(this->deviceName);
        Serial.print("Current Epoch time: ");
        Serial.println(nextEventTime);
        SerialBT.print("In updateDeviceState(). Updating state of ");
        SerialBT.println(this->deviceName);
        SerialBT.print("Current Epoch time: ");
        SerialBT.println(nextEventTime);
    #endif
    if (onTimeInSeconds < offTimeInSeconds) { //device turns on and off within the same 00:00-23:59 period
        if (secondsSinceStartOfDay >= onTimeInSeconds && secondsSinceStartOfDay < offTimeInSeconds) {
            this->turnDeviceOn(powerControl);
            //Calculate Epoch time for device's next off time
            nextEventTime += (offTimeInSeconds - secondsSinceStartOfDay);
        }
        else {
            if (this->deviceName != "airPump" && this->deviceName != "co2") {
                this->turnDeviceOff(powerControl);
            }
            //Calculate Epoch time for device's next on time
            if (secondsSinceStartOfDay < onTimeInSeconds) { //Device has not turned on for the day yet
                nextEventTime += (onTimeInSeconds - secondsSinceStartOfDay); 
            } else { //Device has turned off for the day
                nextEventTime += (onTimeInSeconds + (86400-secondsSinceStartOfDay));
            }
        }
    }
    else if(onTimeInSeconds > offTimeInSeconds) { //Device turns on and off within different 00:00-23:59 periods (ie. device turns on at night and off the next morning)
        if (secondsSinceStartOfDay >= onTimeInSeconds || secondsSinceStartOfDay < offTimeInSeconds) {
            this->turnDeviceOn(powerControl);
            //Calculate Epoch time for device's next off time
            if (secondsSinceStartOfDay >= onTimeInSeconds) { //
                nextEventTime += (offTimeInSeconds + (86400-secondsSinceStartOfDay));
            }
            else {
                nextEventTime += (offTimeInSeconds - secondsSinceStartOfDay);
            }
        }
        else {
            if (this->deviceName != "airPump" && this->deviceName != "co2") {
                this->turnDeviceOff(powerControl);
            }
            //Calculate Epoch time for device's next on time
            nextEventTime += (onTimeInSeconds - secondsSinceStartOfDay);
        }
    }
    else { //onTime and offTime are the same
        if (this->deviceName != "airPump" && this->deviceName != "co2") {
            this->turnDeviceOff(powerControl);
        }
    }
     #ifdef useSerial
        Serial.print("Next event Epoch time: ");
        Serial.println(nextEventTime);
        Serial.println("Leaving updateDeviceState()");
        SerialBT.print("Next event Epoch time: ");
        SerialBT.println(nextEventTime);
        SerialBT.println("Leaving updateDeviceState()");
    #endif
    this->nextEventEpochTime = nextEventTime;
    return;
}
bool TimedDevice::turnDeviceOn(PowerControl *powerControl) {
    if (this->deviceName == "airPump") {
        powerControl->airControl(ON);
        return true;
    }
    if (this->deviceName == "co2") {
        powerControl->airControl(AUXON);
        return true;
    }
    if (this->deviceName == "lights") {
        powerControl->lightControl(ON);
        return true;
    }
    return false;
}
bool TimedDevice::turnDeviceOff(PowerControl *powerControl) {
    if (this->deviceName == "airPump") {
        powerControl->airControl(OFF);
        return true;
    }
    if (this->deviceName == "co2") {
        powerControl->airControl(OFF);
        return true;
    }
    if (this->deviceName == "lights") {
        powerControl->lightControl(OFF);
        return true;
    }
    return false;
}
uint8_t TimedDevice::getOnHr() {
    return this->onHr;
}
uint8_t TimedDevice::getOnMin() {
    return this->onMin;
}
uint8_t TimedDevice::getOffHr() {
    return this->offHr;
}
uint8_t TimedDevice::getOffMin() {
    return this->offMin;
}
void TimedDevice::setOnHr(uint8_t setVal) {
    this->onHr = setVal;
    EEPROM.write(this->onHrSaveLoc, setVal);
}
void TimedDevice::setOnMin(uint8_t setVal) {
    this->onMin = setVal;
    EEPROM.write(this->onMinSaveLoc, setVal);
}
void TimedDevice::setOffHr(uint8_t setVal) {
    this->offHr = setVal;
    EEPROM.write(this->offHrSaveLoc, setVal);
}
void TimedDevice::setOffMin(uint8_t setVal) {
    this->offMin = setVal;
    EEPROM.write(this->offMinSaveLoc, setVal);
}

TimedPowerEventControl::TimedPowerEventControl() {
  
}
void TimedPowerEventControl::init(ESP32Time *rtc, PowerControl *powerControl) {
    this->rtc = rtc;
    this->powerControl = powerControl;
    this->devices[0].init(String("airPump"), AirOnHrLoc, AirOnMinLoc, AirOffHrLoc, AirOffMinLoc, this->rtc, this->powerControl);
    this->devices[1].init(String("co2"), Co2OnHrLoc, Co2OnMinLoc, Co2OffHrLoc, Co2OffMinLoc, this->rtc, this->powerControl);
    this->devices[2].init(String("lights"), LightsOnHrLoc, LightsOnMinLoc, LightsOffHrLoc, LightsOffMinLoc, this->rtc, this->powerControl);
    //this->devices[0].init(String("lights"), EEPROM.read(LightsOnHrLoc), EEPROM.read(LightsOnMinLoc), EEPROM.read(LightsOffHrLoc), EEPROM.read(LightsOffMinLoc), this->rtc, this->powerControl);
    this->numDevices = 3;
    //this->numDevices = 1;
    this->determineDeviceWithNextEvent();
    return;
}
uint8_t TimedPowerEventControl::getNumDevices() {
    return this->numDevices;
}
TimedDevice* TimedPowerEventControl::getDeviceByName(String name) {
    for (int i = 0; i < numDevices; i++) {
        if (this->devices[i].deviceName == name) {
            return &(this->devices[i]);
        }
    }
    return NULL;
}
TimedDevice* TimedPowerEventControl::getDeviceByNumber(uint8_t num) {
    if (!(num > this->numDevices)) {
        return &(this->devices[num]);
    } else {
        return NULL;
    }
}
void TimedPowerEventControl::determineDeviceWithNextEvent() {
    if (numDevices > 0) {
        this->nextDeviceWithEvent = &(this->devices[0]);
        for (int i = 1; i < numDevices; i++) {
            if (this->devices[i].nextEventEpochTime < this->nextDeviceWithEvent->nextEventEpochTime)
                this->nextDeviceWithEvent = &(this->devices[i]);
        }
    }
    #ifdef useSerial
        Serial.print("In determineDeviceWithNextEvent() Next device with event is ");
        Serial.println(this->nextDeviceWithEvent->deviceName);
        Serial.print("Next event Epoch time:");
        Serial.println(this->nextDeviceWithEvent->nextEventEpochTime);
        SerialBT.print("In determineDeviceWithNextEvent() Next device with event is ");
        SerialBT.println(this->nextDeviceWithEvent->deviceName);
        SerialBT.print("Next event Epoch time:");
        SerialBT.println(this->nextDeviceWithEvent->nextEventEpochTime);
    #endif
    return;
}
void TimedPowerEventControl::handleEventTrigger() {
    this->nextDeviceWithEvent->updateDeviceState(this->rtc, this->powerControl);
    this->determineDeviceWithNextEvent();
    return;
}
