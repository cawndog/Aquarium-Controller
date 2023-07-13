#include "SavedStateController.h"

SavedState::SavedState() {
    //constructor
}
void SavedState::init(int* location) {
    *location++;
    this->eepromLoc = *location;
    this->value = EEPROM.read(this->eepromLoc);
}
uint8_t SavedState::getValue() {
    return this->value;
}
void SavedState::saveValue(uint8_t value) {
    if (this->value != value) {
        this->value = value;
        EEPROM.write(this->eepromLoc, value);
        EEPROM.commit();
    }
}

SavedStateController::SavedStateController() {
    this->location = 0;
}
SavedStateController::init() {
    switch1State.init(&location);
    switch2State.init(&location);
    switch3State.init(&location);
    switch4State.init(&location);
    AirOnHr.init(&location);
    AirOnMin.init(&location);
    AirOffHr.init(&location);
    AirOffMin.init(&location);
    Co2OnHr.init(&location);
    Co2OnMin.init(&location);
    Co2OffHr.init(&location);
    Co2OffMin.init(&location);
    LightsOnHr.init(&location);
    LightsOnMin.init(&location);
    LightsOffHr.init(&location);
    LightsOffMin.init(&location);
    aquariumThermostat.init(&location);
}
uint8_t SavedStateController::getSavedStateValue(SavedState* savedState) {
    return savedState->getValue();
}
void SavedStateController::updateSavedStateValue(SavedState* savedState, uint8_t value) {
    savedState->saveValue(value);
}