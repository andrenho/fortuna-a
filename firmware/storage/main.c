#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#include <avr/cpufunc.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "uart.h"

/*
#define get_DATA()     (PIND)
#define set_DATA(v)    { DDRD = 0xff; PORTD = v; }
#define release_DATA() { PORTD = 0x0; DDRD = 0x0; }
*/

#define set_LED(v) { if (v) PORTC |= _BV(PC0); else PORTC &= ~_BV(PC0); }
#define pulse_R() { PORTC |= _BV(PC1); _delay_ms(1); PORTC &= ~_BV(PC1); _delay_ms(1); }

volatile bool y2r = false;
volatile bool y2w = false;

static void pulse_LED(int times)
{
    for (int i = 0; i < times; ++i) {
        set_LED(1);
        _delay_ms(30);
        set_LED(0);
        _delay_ms(250);
    }
}

int main()
{
    DDRC |= _BV(PC0) | _BV(PC1);   // LED and R
    PORTD |= _BV(PD2) | _BV(PD3);  // pullups on Y2R and Y2W

    pulse_R();

    EICRA |= _BV(ISC01) | _BV(ISC11);  // falling edge triggers INT0 and INT1 (Y2R, Y2W)
    EIMSK |= _BV(INT0) | _BV(INT1);    // enable interrupts INT0, INT1
    sei();

    uart_init();
    printf("Initialized.\n");

    for (;;) {

        if (y2r) {
            y2r = false;
            while (PIND & _BV(PD2) == 0) {
                pulse_R();
            }
        }

        if (y2w) {
            y2w = false;
            // pulse_R();
        }
    }
}

ISR(INT0_vect)  // on Y2R falling
{
    y2r = true;
}

ISR(INT1_vect)  // on Y2W falling
{
    y2w = true;
}
