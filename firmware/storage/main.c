#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#include <avr/cpufunc.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "output.h"
#include "sdcard.h"
#include "spi.h"
#include "uart.h"

#define get_Y2W() (PIND & _BV(PIND3))

#define set_LED(v) { if (v) PORTC |= _BV(PC0); else PORTC &= ~_BV(PC0); }
#define pulse_R() { PORTC |= _BV(PC1); _delay_ms(1); PORTC &= ~_BV(PC1); _delay_ms(1); }

volatile bool y2w = false;
volatile uint8_t last_data = 0;

static void pulse_LED(int times)
{
    for (int i = 0; i < times; ++i) {
        set_LED(1);
        _delay_ms(30);
        set_LED(0);
        _delay_ms(250);
    }
}

static inline uint8_t get_data()
{
    uint8_t pd = PIND, pc = PINC;
    return ((pc >> 2) & 0xf) | (pd & 0xf0);
}

#define set_CE()   { PORTB |= _BV(PB1); /* debug_spi_inactive(PSTR("SD")); */ }
#define clear_CE() { PORTB &= ~_BV(PB1); /* debug_spi_active(PSTR("SD")); */ }

int main()
{
    spi_init();
    output_init();
    sdcard_init();

    uart_init();
    getchar();
    printf("\e[1;1H\e[2JInitialized.\n");

    sdcard_init();

    sdcard_setup();

    printf("Done.\n");
    for (;;) ;

#if 0
    DDRC = _BV(PC0) | _BV(PC1);   // LED and R

    pulse_R();
    pulse_R();

    EICRA |= _BV(ISC11);   // falling edge triggers INT1 (Y2R)
    EIMSK |= _BV(INT1);    // enable interrupts INT1
    sei();

    for (;;) {

        if (y2w) {
            y2w = false;
            last_data = get_data();
            output_set(last_data);
            pulse_R();
            pulse_R();
            // printf("%02X ", last_data);
        }
    }
#endif
}

ISR(INT1_vect)  // on Y2W falling
{
    y2w = true;
}
