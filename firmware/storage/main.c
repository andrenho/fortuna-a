#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#include <avr/cpufunc.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "output.h"
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

int main()
{
    DDRC = _BV(PC0) | _BV(PC1);   // LED and R

    spi_init();
    output_init();

    pulse_R();
    pulse_R();

    EICRA |= _BV(ISC11);   // falling edge triggers INT1 (Y2R)
    EIMSK |= _BV(INT1);    // enable interrupts INT1
    sei();

    for (;;) {

        if (y2w) {
            y2w = false;
            last_data = get_data();
            output_set(last_data + 1);
            pulse_R();
            pulse_R();
        }
    }
}

ISR(INT1_vect)  // on Y2W falling
{
    y2w = true;
}
