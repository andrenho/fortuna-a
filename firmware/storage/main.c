#include <stdio.h>
#include <stdint.h>

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "uart.h"

#if 0
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

static void init_spi(void)
{
    USICR = _BV(USIWM0) | _BV(USICS1);  // 3-wire mode, clock from timer overflow
    USISR = _BV(USIOIF);  // timer overflow interrupt

    sei();
}

static void send_via_spi(uint8_t data)
{
    PORTD &= ~_BV(PORTD4);

    USIDR = data;
    USISR = _BV(USIOIF);

    // wait for a complete transfer
    while (!(USISR & _BV(USIOIF))) {
        USICR |= _BV(USITC);   // toggle clock
    }

    PORTD |= _BV(PORTD4);
}

int main(void)
{
    uart_init();
    init_spi();
    puts_P(PSTR("\e[1;1H\e[2JHello!"));

    // setup ports
    DDRD |= _BV(DDD2) | _BV(DDD4);  // R, SD_CS = output
    PORTD |= _BV(PORTD4);

    // initial setup
    reset_flip_flop();

    // main loop
    /*
    for (;;) {
        while (PIND & _BV(PD3));  // wait until Y2W = 0

        uint8_t data = read_data();
        send_via_spi(data);
        
        // TODO - something with RCLK?

        reset_flip_flop();
    }
    */

    for (;;) {
        send_via_spi(0xaf);
        _delay_ms(1);
    }

    for (;;);

    return 0;
}
#endif

int main()
{
    DDRB |= _BV(DDB7) | _BV(DDB6);  // SCLK, DO = output
    DDRD |= _BV(DDD2) | _BV(DDD4);  // R, SD_CS = output
    PORTD |= _BV(PORTD4);

    for (;;) {
        PORTD &= ~_BV(PORTD4);

        USIDR = 0xab;

        // Configure SPI settings for MSB
        uint8_t usicr_msb = (1 << USIWM0) | (0 << USICS0) | (1 << USITC);
        
        // Configure SPI settings for LSB
        uint8_t usicr_lsb = (1 << USIWM0) | (0 << USICS0) | (1 << USITC) | (1 << USICLK);

        // SPI transfer loop for 8 bits
        for (int i = 0; i < 16; i++) {
            if (i % 2 == 0) {
                USICR = usicr_msb;
            } else {
                USICR = usicr_lsb;
            }
        }

        PORTD |= _BV(PORTD4);

        _delay_ms(1);
    }

}
