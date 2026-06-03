TARGET = firmware

CC = arm-none-eabi-gcc

CFLAGS = \
-mcpu=cortex-m4 \
-mthumb \
-mfpu=fpv4-sp-d16 \
-mfloat-abi=hard \
-O0 \
-g3 \
-DSTM32F411xE \
-Wall \
-IInc \
-ICMSIS/Include \
-ICMSIS/Device/ST/STM32F4xx/Include

LDFLAGS = \
-TSTM32F411RETX_FLASH.ld \
-mcpu=cortex-m4 \
-mthumb \
-mfpu=fpv4-sp-d16 \
-mfloat-abi=hard

C_SOURCES = \
$(wildcard Src/*.c)

ASM_SOURCES = \
Startup/startup_stm32f411retx.s

all:
	$(CC) $(CFLAGS) $(C_SOURCES) $(ASM_SOURCES) $(LDFLAGS) -o $(TARGET).elf

clean:
	del /Q *.elf
