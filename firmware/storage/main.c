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

static inline void set_data(uint8_t v)
{
    DDRC |= _BV(PC2) | _BV(PC3) | _BV(PC4) | _BV(PC5);
    DDRD |= _BV(PD4) | _BV(PD5) | _BV(PD6) | _BV(PD7);
    PORTC |= (PORTC & 0b11000011) | ((v & 0xf) << 2);
    PORTD |= (PORTD & 0b11110000) | ((v >> 4) & 0xf0);
}

static inline void release_data()
{
    DDRC &= ~(_BV(PC2) | _BV(PC3) | _BV(PC4) | _BV(PC5));
    DDRD &= ~(_BV(PD4) | _BV(PD5) | _BV(PD6) | _BV(PD7));
}

int main()
{
    DDRC |= _BV(PC0) | _BV(PC1);   // LED and R
    PORTD |= _BV(PD2) | _BV(PD3);  // pullups on Y2R and Y2W

    pulse_R();
    pulse_R();

    EICRA |= _BV(ISC01) | _BV(ISC11);  // falling edge triggers INT0 and INT1 (Y2R, Y2W)
    EIMSK |= _BV(INT0) | _BV(INT1);    // enable interrupts INT0, INT1
    sei();

    uart_init();
    printf("Initialized.\n");

    for (;;) {

        if (y2r) {
            set_data(last_data + 1);
            y2r = false;
            putchar('A');
            while (PIND & _BV(PIND2) == 0) {
                putchar('B');
            }
            putchar('C');
            pulse_R();
            release_data();
            putchar('D');
        }

        if (y2w) {
            y2w = false;
            last_data = get_data();
            printf("%X ", last_data);
            pulse_R();
            pulse_R();
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
