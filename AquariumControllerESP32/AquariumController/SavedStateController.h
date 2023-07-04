
#include "EEPROM.h"

class SavedState {
    private: 
        uint8_t eepromLoc;
        uint8_t value;
    public:
        SavedState();
        void init(int* location);
        uint8_t getValue();
        void saveValue(uint8_t value);
};

class SavedStateController {
    private:
        int location;

    public:
        SavedState switch1State;
        SavedState switch2State;
        SavedState switch3State;
        SavedState switch4State;
        SavedState AirOnHr;
        SavedState AirOnMin;
        SavedState AirOffHr;
        SavedState AirOffMin;
        SavedState Co2OnHr;
        SavedState Co2OnMin;
        SavedState Co2OffHr;
        SavedState Co2OffMin;
        SavedState LightsOnHr;
        SavedState LightsOnMin;
        SavedState LightsOffHr;
        SavedState LightsOffMin;
        SavedState aquariumThermostat;

        SavedStateController();
        void init();
        uint8_t getSavedStateValue(SavedState * savedState);
        void updateSavedStateValue(SavedState* savedState, uint8_t value);
};