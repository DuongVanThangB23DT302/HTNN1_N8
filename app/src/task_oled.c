#include "task_oled.h"
#include "app_ipc.h"
#include "app_cfg.h"
#include "ssd1306.h"
#include <stdio.h>

static void task_oled_run(void *argument)
{
    (void)argument;
    bool ready = false;
    for (;;) {
        Status status;
        xQueuePeek(oled_queue, &status, portMAX_DELAY);
        if (!ready) ready = ssd1306_init();
        if (ready) {
            char text[22];
            ssd1306_clear();
            snprintf(text, sizeof text, "MQ-2 ADC: %u", status.sensors.mq_adc);
            ssd1306_text(0, 0, text);
            snprintf(text, sizeof text, "LUA ADC: %u", status.sensors.flame_adc);
            ssd1306_text(1, 0, text);
            snprintf(text, sizeof text, "NGUONG MQ: %u", status.threshold);
            ssd1306_text(2, 0, text);
            snprintf(text, sizeof text, "NGUONG LUA: %u", status.flame_threshold);
            ssd1306_text(3, 0, text);
            ssd1306_text(4, 0, status.flame ? "PHAT HIEN LUA" :
                         status.gas ? "PHAT HIEN KHI" :
                         status.pending ? "DANG DEM TRE" : "BINH THUONG");
            ssd1306_text(5, 0, status.alarm_on ? "LED COI: BAT" : "LED COI: TAT");
            ssd1306_text(6, 0, status.fan_on ? "QUAT: BAT" : "QUAT: TAT");
            snprintf(text, sizeof text, "TRE: %u S", status.delay_seconds);
            ssd1306_text(7, 0, text);
            ready = ssd1306_show();
        }
        vTaskDelay(pdMS_TO_TICKS(250));
    }
}

BaseType_t task_oled_create(void)
{
    return xTaskCreate(task_oled_run, "OLED", OLED_STACK_WORDS,
                       NULL, OLED_PRIORITY, NULL);
}
