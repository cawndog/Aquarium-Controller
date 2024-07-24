#include "WString.h"
#include "Switch.h"

/*static void _set_angle(ledc_mode_t speed_mode, float angle)
{
    for (size_t i = 0; i < 8; i++) {
        iot_servo_write_angle(speed_mode, i, angle);
    }
}*/
servo_config_t servo_cfg = {
    .max_angle = 180,
    .min_width_us = 544,
    .max_width_us = 2400,
    .freq = 50,
    .timer_number = LEDC_TIMER_0,
    .channels = {
        .servo_pin = {
            gpio_num_t(SERVO_CH0_PIN),
            gpio_num_t(SERVO_CH1_PIN),
            gpio_num_t(SERVO_CH2_PIN),
            gpio_num_t(SERVO_CH3_PIN)
        },
        .ch = {
            LEDC_CHANNEL_0,
            LEDC_CHANNEL_1,
            LEDC_CHANNEL_2,
            LEDC_CHANNEL_3,
        },
    },
    .channel_number = 4,
} ;

void Switch::init(String name) {
  if (switchSemaphore == NULL) {
    switchSemaphore = xSemaphoreCreateBinary();
    xSemaphoreGive(switchSemaphore);
  }
  this->name = name;
  String switchStateKey = this->name + "_SW";
  this->state = this->IntToSwitchState(savedState.getUChar(switchStateKey.c_str(), 0));
}
SwitchState Switch::getSwitchState() {
  return this->state;
}
uint8_t Switch::SwitchStateToInt() {
  switch (this->state) {
    case OFF:
      return 0;
    case ON: 
      return 1;
    case AUXON:
      return 2;
  };
  return 0;
}
SwitchState Switch::IntToSwitchState(uint8_t state) {
  switch (state) {
    case 0:
      return OFF;
    case 1: 
      return ON;
    case 2:
      return AUXON;
    default: 
      return OFF;
  };
}
void Switch::setSwitchState(SwitchState newState) {
  this->state = newState;
  String switchStateKey = this->name + "_SW";
  savedState.putUChar(switchStateKey.c_str(), this->SwitchStateToInt());
  return;
}
void Switch1::powerControl(SwitchState newState) { //air control switch
  //ON POS is 70 (POS1)
  //AUXON POS is 110 (POS2)
  //ON POS = Air Pump on
  //AUXON POS = CO2 on
  #define OFF_OFFSET_1 -8
  #define ON_OFFSET_1 0
  #define AUXON_OFFSET_1 -9

  if (this->state != newState) {
    this->setSwitchState(newState);
    xSemaphoreTake(switchSemaphore, portMAX_DELAY);
    if (activeServoTasks == 0) {
      iot_servo_init(LEDC_HIGH_SPEED_MODE, &servo_cfg);
    }
    activeServoTasks++;
    xSemaphoreGive(switchSemaphore);
    if (newState == OFF) {
      iot_servo_write_angle(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, OFF_POS + OFF_OFFSET_1);
      const TickType_t xDelay = 200 / portTICK_PERIOD_MS;
      vTaskDelay(xDelay);
      //iot_servo_deinit(LEDC_HIGH_SPEED_MODE);
    }
    else if (newState == ON) { //Air Pump
      iot_servo_write_angle(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, POS1 + ON_OFFSET_1);
      const TickType_t xDelay = 200 / portTICK_PERIOD_MS;
      vTaskDelay(xDelay);
      //iot_servo_deinit(LEDC_HIGH_SPEED_MODE);
    }
    else if (newState == AUXON) { //CO2
      iot_servo_write_angle(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, POS2 + AUXON_OFFSET_1);
      const TickType_t xDelay = 200 / portTICK_PERIOD_MS;
      vTaskDelay(xDelay);
      //iot_servo_deinit(LEDC_HIGH_SPEED_MODE);
    }

    xSemaphoreTake(switchSemaphore, portMAX_DELAY);
    activeServoTasks--;
    if (activeServoTasks == 0) {
      iot_servo_deinit(LEDC_HIGH_SPEED_MODE);
      const TickType_t xDelay = 100 / portTICK_PERIOD_MS;
      vTaskDelay(xDelay);
    }
    xSemaphoreGive(switchSemaphore);
  }
  return;
}
void Switch2::powerControl(SwitchState newState) { //light control switch
  //ON POS is 110 (POS2) 
  //AUXON POS is 70 (POS1)
  //On POS = Lights on
  //AUXON POS = Not Configured
  #define OFF_OFFSET_2 1
  #define ON_OFFSET_2 -2
  #define AUXON_OFFSET_2 3
  
  if (this->state != newState) {
    this->setSwitchState(newState);
    xSemaphoreTake(switchSemaphore, portMAX_DELAY);
    if (activeServoTasks == 0) {
      iot_servo_init(LEDC_HIGH_SPEED_MODE, &servo_cfg);
    }
    activeServoTasks++;
    xSemaphoreGive(switchSemaphore);
    if (newState == OFF) {
      iot_servo_write_angle(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3, OFF_POS + OFF_OFFSET_2);
      const TickType_t xDelay = 200 / portTICK_PERIOD_MS;
      vTaskDelay(xDelay);
      //iot_servo_deinit(LEDC_HIGH_SPEED_MODE);
    }
    else if (newState == ON) { 
      iot_servo_write_angle(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3, POS2 + ON_OFFSET_2);
      const TickType_t xDelay = 200 / portTICK_PERIOD_MS;
      vTaskDelay(xDelay);
      //iot_servo_deinit(LEDC_HIGH_SPEED_MODE);
    }
    /*else if (newState == AUXON) { 
      iot_servo_write_angle(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3, POS1 + AUXON_OFFSET_2);
      const TickType_t xDelay = 200 / portTICK_PERIOD_MS;
      vTaskDelay(xDelay);
      //iot_servo_deinit(LEDC_HIGH_SPEED_MODE);
    }*/
    xSemaphoreTake(switchSemaphore, portMAX_DELAY);
    activeServoTasks--;
    if (activeServoTasks == 0) {
      iot_servo_deinit(LEDC_HIGH_SPEED_MODE);
      const TickType_t xDelay = 100 / portTICK_PERIOD_MS;
      vTaskDelay(xDelay);
    }
    xSemaphoreGive(switchSemaphore);
  }
  return;
}
void Switch3::powerControl(SwitchState newState) { //heater control switch
  //ON POS is 70 (POS1)
  //AUXON POS is 110 (POS2)
  //On POS = Heater on
  //AUXON POS = Not Configured
  #define OFF_OFFSET_3 0
  #define ON_OFFSET_3 0
  #define AUXON_OFFSET_3 -2
  if (this->state != newState) {
    this->setSwitchState(newState);
    xSemaphoreTake(switchSemaphore, portMAX_DELAY);
    if (activeServoTasks == 0) {
      iot_servo_init(LEDC_HIGH_SPEED_MODE, &servo_cfg);
    }
    activeServoTasks++;
    xSemaphoreGive(switchSemaphore);
    if (newState == OFF) {
      iot_servo_write_angle(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, OFF_POS + OFF_OFFSET_3);
      const TickType_t xDelay = 200 / portTICK_PERIOD_MS;
      vTaskDelay(xDelay);
      //iot_servo_deinit(LEDC_HIGH_SPEED_MODE);
    }
    else if (newState == ON) { 
      iot_servo_write_angle(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, POS1 + ON_OFFSET_3);
      const TickType_t xDelay = 200 / portTICK_PERIOD_MS;
      vTaskDelay(xDelay);
      //iot_servo_deinit(LEDC_HIGH_SPEED_MODE);
    }
    /*else if (newState == AUXON) { 
      iot_servo_write_angle(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, POS2 + AUXON_OFFSET_3);
      const TickType_t xDelay = 200 / portTICK_PERIOD_MS;
      vTaskDelay(xDelay);
      //iot_servo_deinit(LEDC_HIGH_SPEED_MODE);
    }*/
    xSemaphoreTake(switchSemaphore, portMAX_DELAY);
    activeServoTasks--;
    if (activeServoTasks == 0) {
      iot_servo_deinit(LEDC_HIGH_SPEED_MODE);
      const TickType_t xDelay = 100 / portTICK_PERIOD_MS;
      vTaskDelay(xDelay);
    }
    xSemaphoreGive(switchSemaphore);
  }
  return;
}
void Switch4::powerControl(SwitchState newState) { //filter control switch
  //ON POS is 110 (POS2) 
  //AUXON POS is 70 (POS1)
  //On POS = Filter on
  //AUXON POS = Not Configured
  #define OFF_OFFSET_4 10
  #define ON_OFFSET_4 10
  #define AUXON_OFFSET_4 10
  if (this->state != newState) {
    this->setSwitchState(newState);
    xSemaphoreTake(switchSemaphore, portMAX_DELAY);
    if (activeServoTasks == 0) {
      iot_servo_init(LEDC_HIGH_SPEED_MODE, &servo_cfg);
    }
    activeServoTasks++;
    xSemaphoreGive(switchSemaphore);
    if (newState == OFF) {
      iot_servo_write_angle(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, OFF_POS + OFF_OFFSET_4);
      const TickType_t xDelay = 200 / portTICK_PERIOD_MS;
      vTaskDelay(xDelay);
      //iot_servo_deinit(LEDC_HIGH_SPEED_MODE);
    }
    else if (newState == ON) {
      iot_servo_write_angle(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, POS2 + ON_OFFSET_4);
      const TickType_t xDelay = 200 / portTICK_PERIOD_MS;
      vTaskDelay(xDelay);
      //iot_servo_deinit(LEDC_HIGH_SPEED_MODE);
    }
    /*else if (newState == AUXON) { 
      iot_servo_write_angle(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, POS1 + AUXON_OFFSET_4);
      const TickType_t xDelay = 200 / portTICK_PERIOD_MS;
      vTaskDelay(xDelay);
      //iot_servo_deinit(LEDC_HIGH_SPEED_MODE);
    }*/
    xSemaphoreTake(switchSemaphore, portMAX_DELAY);
    activeServoTasks--;
    if (activeServoTasks == 0) {
      iot_servo_deinit(LEDC_HIGH_SPEED_MODE);
      const TickType_t xDelay = 100 / portTICK_PERIOD_MS;
      vTaskDelay(xDelay);
    }
    xSemaphoreGive(switchSemaphore);
  }
  return;
}