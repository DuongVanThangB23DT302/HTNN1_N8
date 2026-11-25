TEST ?= system
MODE_system := 0
MODE_sensors := 1
MODE_buttons := 2
MODE_control := 3
MODE_oled := 4
MODE_uart := 5
MODE_relay := 6
TEST_MODE := $(MODE_$(TEST))
ifeq ($(TEST_MODE),)
$(error Unknown TEST: $(TEST))
endif

TASKS_system := sensors buttons control oled uart
TASKS_sensors := sensors uart
TASKS_buttons := buttons control
TASKS_control := sensors control uart
TASKS_oled := oled
TASKS_uart := control uart
TASKS_relay :=
ENTRY := $(if $(filter system,$(TEST)),main.c,tests/$(TEST)/main.c)
BUILD_DIR := Output/$(TEST)
ifeq ($(OS),Windows_NT)
SHELL := bash
endif

RELAY_ACTIVE_HIGH ?= 1
BUZZER_ACTIVE_HIGH ?= 1
OLED_ADDRESS ?= 0x3C
FLASH_METHOD ?= cube
ST_FLASH ?= st-flash

CROSS ?= arm-none-eabi-
CC := $(CROSS)gcc
OBJCOPY := $(CROSS)objcopy
SIZE := $(CROSS)size
FLASH_TOOL ?= STM32_Programmer_CLI

TARGET := $(BUILD_DIR)/firmware
ARCH := -mcpu=cortex-m3 -mthumb
RTOS := middlewares/FreeRTOS
PORT := $(RTOS)/portable/GCC/ARM_CM3
DEVICE := vendor/stm32f10x_lib
CMSIS := vendor/cmsis/CMSIS/Core/Include
LINKER := bsp/linker/stm32f103c8.ld

CPPFLAGS += -DSTM32F103xB -DTEST_MODE=$(TEST_MODE) -DRELAY_ACTIVE_HIGH=$(RELAY_ACTIVE_HIGH) -DBUZZER_ACTIVE_HIGH=$(BUZZER_ACTIVE_HIGH) -DOLED_ADDRESS=$(OLED_ADDRESS) -Iapp/inc -Ibsp -Ihal/inc -Imiddlewares/config -I$(RTOS)/include \
            -I$(PORT) -I$(DEVICE)/Include -I$(CMSIS)
CFLAGS += $(ARCH) -std=c11 -Os -g3 -Wall -Wextra -ffunction-sections -fdata-sections
LDFLAGS += $(ARCH) -nostartfiles --specs=nano.specs --specs=nosys.specs \
           -T$(LINKER) -Wl,--gc-sections,-Map=$(TARGET).map

C_SRC := $(ENTRY) app/src/app_ipc.c app/src/app_fault.c \
         $(foreach task,$(TASKS_$(TEST)),app/src/task_$(task).c) \
         $(wildcard bsp/*.c hal/src/*.c bsp/startup/*.c) \
         $(DEVICE)/Source/Templates/system_stm32f1xx.c \
         $(RTOS)/tasks.c $(RTOS)/queue.c $(RTOS)/list.c \
         $(PORT)/port.c $(RTOS)/portable/MemMang/heap_4.c
ASM_SRC := bsp/startup/startup_stm32f103xb.s
OBJECTS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(C_SRC)) $(patsubst %.s,$(BUILD_DIR)/%.o,$(ASM_SRC))

.PHONY: all clean flash
all: $(TARGET).elf $(TARGET).hex $(TARGET).bin
	$(SIZE) $(TARGET).elf

$(TARGET).elf: $(OBJECTS) $(LINKER)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LDLIBS) -o $@

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex $< $@

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

$(BUILD_DIR)/%.o: %.c
	@mkdir -p "$(@D)"
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR)/%.o: %.s
	@mkdir -p "$(@D)"
	$(CC) $(ARCH) -c $< -o $@

ifeq ($(FLASH_METHOD),stlink)
flash: $(TARGET).bin
	"$(ST_FLASH)" --reset write "$<" 0x08000000
else
flash: $(TARGET).hex
	"$(FLASH_TOOL)" -c port=SWD -w "$<" -v -rst
endif

clean:
	rm -rf "$(BUILD_DIR)"

-include $(OBJECTS:.o=.d)
