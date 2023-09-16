#include <stdio.h>
#include <stdint.h>

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
    pinb |= ((pind >> 1) & 0b11000000);
    return pinb;
}

static void send_via_spi(uint8_t data)
{
    // TODO
}

int main(void)
{
    uart_init();
    puts_P(PSTR("\e[1;1H\e[2JHello!"));

    // setup ports
    DDRD |= _BV(DDD2) | _BV(DDD4);  // R, SD_CS = output

    // setup SPI (TODO)
    
    // initial setup
    reset_flip_flop();

    // main loop
    for (;;) {
        while (PIND & _BV(PD3));  // wait until Y2W = 0

        uint8_t data = read_data();
        send_via_spi(data);
        
        // TODO - something with RCLK?

        reset_flip_flop();
    }

    for (;;);

    return 0;
}
