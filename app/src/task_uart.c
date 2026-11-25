#include "task_uart.h"
#include "app_ipc.h"
#include "hal_uart.h"
#include "app_cfg.h"
#include <stdio.h>

#if TEST_MODE != 1

static void parse_line(const char *line, unsigned length)
{
    if (length < 4U || line[2] != '=') return;

    unsigned value = 0;
    for (unsigned i = 3; i < length; ++i) {
        if (line[i] < '0' || line[i] > '9') return;
        value = 10U * value + (unsigned)(line[i] - '0');
        if (value > 4095U) return;
    }

    Command command = { .value = value };
    if (line[0] == 'T' && line[1] == 'H') {
        command.type = SET_THRESHOLD;
    } else if (line[0] == 'F' && line[1] == 'T') {
        command.type = SET_FLAME_THRESHOLD;
    } else if (line[0] == 'D' && line[1] == 'E' && value <= MAX_DELAY_SECONDS) {
        command.type = SET_DELAY;
    } else if (line[0] == 'A' && line[1] == 'L' && value <= 1U) {
        command.type = SET_ALARM;
    } else if (line[0] == 'F' && line[1] == 'A' && value <= 1U) {
        command.type = SET_FAN;
    } else {
        return;
    }
    xQueueSend(command_queue, &command, portMAX_DELAY);
}

#endif

static void send_status(const Status *status)
{
    char report[64];
    snprintf(report, sizeof report, "S,%u,%u,%d,%d,%d,%d,%d,%d,%u,%u,%u\r\n",
             status->sensors.mq_adc, status->sensors.flame_adc,
             status->gas, status->flame, status->alarm_on, status->fan_on,
             status->alarm_manual, status->fan_manual, status->threshold,
             status->flame_threshold, status->delay_seconds);
    for (const char *ch = report; *ch; ++ch) hal_uart_put(*ch);
}

static void task_uart_run(void *argument)
{
    (void)argument;
#if TEST_MODE != 1
    char line[24];
    unsigned length = 0;
    bool discard = false;
#endif
    TickType_t last_report = xTaskGetTickCount();

    for (;;) {
#if TEST_MODE != 1
        for (unsigned count = 0; count < 96U; ++count) {
            int ch = hal_uart_get();
            if (ch == -1) break;
            if (ch == -2) {
                discard = true;
                length = 0;
                continue;
            }
            if (ch == '\n') {
                if (!discard) parse_line(line, length);
                length = 0;
                discard = false;
            } else if (ch != '\r') {
                if (length < sizeof line) line[length++] = (char)ch;
                else discard = true;
            }
        }

#endif
        TickType_t now = xTaskGetTickCount();
        if (now - last_report >= pdMS_TO_TICKS(1000)) {
            Status status;
            if (xQueueReceive(uart_queue, &status, 0) == pdPASS) {
                send_status(&status);
                last_report = now;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(2));
    }
}

BaseType_t task_uart_create(void)
{
    return xTaskCreate(task_uart_run, "UART", UART_STACK_WORDS,
                       NULL, UART_PRIORITY, NULL);
}
