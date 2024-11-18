#ifndef SWITCH_H
#define SWITCH_H
//#include <ESP32Servo.h>
#include <Preferences.h>
#include "AquariumController.h"

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "iot_servo.h"
#include "unity.h"
#include "sdkconfig.h"

#if SOC_LEDC_SUPPORT_HS_MODE
  #define SRVO_LEDC_SPEED_MODE LEDC_HIGH_SPEED_MODE
#else
  #define SRVO_LEDC_SPEED_MODE LEDC_LOW_SPEED_MODE
#endif

#define SERVO_CH0_PIN HEATER_SRVO_PIN //Heater Servo, Servo #3
#define SERVO_CH1_PIN AIR_SRVO_PIN //Air Servo, Servo #1
#define SERVO_CH2_PIN FILTER_SRVO_PIN //Filter Servo, Servo #4
#define SERVO_CH3_PIN LIGHT_SRVO_PIN //Light Servo, Servo #2

//--Servo Motor Positions--
#define OFF_POS 90
#define POS1 70
#define POS2 110
static volatile SemaphoreHandle_t switchSemaphore = NULL;
static volatile uint8_t activeServoTasks = 0;
extern Preferences savedState;
enum SwitchState {
  OFF = 0,
  ON = 1,
  AUXON = 2
};
class Switch {
  public: 
    String name;
    SwitchState state;
    //Servo controllerServo;
    void init(String name);
    void powerControl(SwitchState newState) {};
    void setSwitchState(SwitchState newState);
    SwitchState getSwitchState();
    uint8_t SwitchStateToInt();
    SwitchState IntToSwitchState(uint8_t state);
};

class Switch1: public Switch {
  public: 
    void powerControl(SwitchState newState);
};
class Switch2: public Switch {
  public: 
    void powerControl(SwitchState newState);
};
class Switch3: public Switch {
  public: 
    void powerControl(SwitchState newState);
};
class Switch4: public Switch {
  public:
    void powerControl(SwitchState newState);
};

#endif