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
    SavedState airState.init(&location);
    SavedState lightState.init(&location);
    SavedState heaterState.init(&location);
    SavedState filterState.init(&location);
    SavedState AirOnHr.init(&location);
    SavedState AirOnMin.init(&location);
    SavedState AirOffHr.init(&location);
    SavedState AirOffMin.init(&location);
    SavedState Co2OnHr.init(&location);
    SavedState Co2OnMin.init(&location);
    SavedState Co2OffHr.init(&location);
    SavedState Co2OffMin.init(&location);
    SavedState LightsOnHr.init(&location);
    SavedState LightsOnMin.init(&location);
    SavedState LightsOffHr.init(&location);
    SavedState LightsOffMin.init(&location);
    SavedState aquariumThermostat.init(&location);
}
uint8_t SavedStateController::getSavedStateValue(SavedState* savedState) {
    return savedState->getValue();
}
void SavedStateController::updateSavedStateValue(SavedState* savedState, uint8_t value) {
    savedState->saveValue(value);
}