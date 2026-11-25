#ifndef APP_IPC_H
#define APP_IPC_H

#include <stdbool.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

typedef struct {
    unsigned mq_adc;
    unsigned flame_adc;
} Sensors;

typedef enum {
    TOGGLE_ALARM, TOGGLE_FAN, SET_ALARM, SET_FAN, SET_THRESHOLD, SET_FLAME_THRESHOLD, SET_DELAY
} CommandType;

typedef struct {
    CommandType type;
    unsigned value;
} Command;

typedef struct {
    Sensors sensors;
    unsigned threshold, flame_threshold, delay_seconds;
    bool pending, gas, flame, alarm_manual, fan_manual, alarm_on, fan_on;
} Status;

extern QueueHandle_t sensor_queue, command_queue, oled_queue, uart_queue;
enum { COMMAND_CAPACITY = 12 };

void app_ipc_init(void);

#endif
