#include <stdio.h>
#include <stdint.h>

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "uart.h"

static void reset_flip_flop(void)
{
    PORTD |= _BV(PD2);
    PORTD &= ~_BV(PD2);
}

inline static uint8_t read_data(void)
{
    uint8_t pinb = PINB;
    uint8_t pind = PIND;
    pinb &= 0b11111;
    if (pind & _BV(0))
        pinb |= 0b100000;
    pinb |= ((pind << 1) & 0b11000000);
    return pinb;
}

void spi_xfer(uint8_t data)
{
    USIDR = data;

    uint8_t usicr_msb = (1 << USIWM0) | (0 << USICS0) | (1 << USITC);
    uint8_t usicr_lsb = (1 << USIWM0) | (0 << USICS0) | (1 << USITC) | (1 << USICLK);

    // SPI transfer loop for 8 bits
    PORTD &= ~_BV(PD4);  // CS = 0
    for (int i = 0; i < 16; i++) {
        if (i % 2 == 0) {
            USICR = usicr_msb;
        } else {
            USICR = usicr_lsb;
        }
    }
    PORTD |= _BV(PD4);  // CS = 1
}

int main(void)
{
    // initialize
    DDRB |= _BV(DDB7) | _BV(DDB6);  // SCLK, DO = output
    DDRD |= _BV(DDD1) | _BV(DDD2) | _BV(DDD4);  // RCLK, R, SD_CS = output
    PORTD |= _BV(PD4);  // SD_CS = 1

    for (;;) {
        reset_flip_flop();

        while (PIND & _BV(PD3));  // wait until Y2W = 0

        // send byte to SDCard (DO)
        //   (SDCard output (MISO) is sent to 74HC595)
        uint8_t data = read_data();
        spi_xfer(data);
        
        // update output register (RCLK at 74HC595)
        PORTD |= _BV(PD1);
        PORTD &= ~_BV(PD1);
    }
}
