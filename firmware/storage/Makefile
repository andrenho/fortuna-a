# Project name
PROJECT=fb-storage

# Object files (one for each source file)
OBJECTS=main.o uart.o output.o spi.o

# Avrdude configuration to connect on the PI. See avrdude_gpio.conf.
AVRDUDE_CONFIG=pi_rst

# AVR fuses: external clock
FUSES=-U lfuse:w:0xff:m -U hfuse:w:0xd9:m -U efuse:w:0xff:m

# Microcontroller
MCU_GCC=atmega328
MCU_AVRDUDE=m328p

# Microcontroller speed, in Hz. Used by avr-libc for delays.
F_CPU=16500000UL

# Serial speed, in baud. Used by avr-libc for UART speed calculation.
BAUD=9600
SERIAL_PORT=/dev/ttyS0

#
# Rules
#

AVRDUDE_FLAGS=-p ${MCU_AVRDUDE} -C +./avrdude_gpio.conf -c ${AVRDUDE_CONFIG} -V
CC=avr-gcc
WARNINGS=-Wall -Wextra \
	 -Wformat=2 -Wno-unused-parameter -Wshadow \
	 -Wwrite-strings \
	 -Wredundant-decls -Wnested-externs -Wmissing-include-dirs -Wjump-misses-init -Wlogical-op
CPPFLAGS=-std=c11 ${WARNINGS} -O3 -DF_CPU=${F_CPU} -DBAUD=${BAUD} -mmcu=${MCU_GCC} -ffunction-sections -fdata-sections -mcall-prologues

all: ${PROJECT}.hex

${PROJECT}.hex: ${PROJECT}.elf
	avr-objcopy -j .text -j .data -O ihex $< $@
	arm-linux-gnueabihf-size ${PROJECT}.hex

${PROJECT}.elf: ${OBJECTS}
	$(CC) -mmcu=${MCU_GCC} -o $@ $^ -Wl,--gc-sections
	avr-size -C --mcu=${MCU_GCC} ${PROJECT}.elf	

test-connection:
	sudo avrdude ${AVRDUDE_FLAGS}

upload: ${PROJECT}.hex
	sudo avrdude ${AVRDUDE_FLAGS} -U flash:w:$<:i

fuse:
	sudo avrdude ${AVRDUDE_FLAGS} ${FUSES}

size: ${PROJECT}.elf
	avr-size -C --mcu=${MCU_GCC} ${PROJECT}.elf	

talk:
	@echo "Press Ctrl+A and then Shift+K to exit."
	@sleep 1
	screen ${SERIAL_PORT} ${BAUD}

clean:
	rm -f ${OBJECTS} ${PROJECT}.elf ${PROJECT}.hex

# vim: set ts=8 sts=8 sw=8 noexpandtab:
