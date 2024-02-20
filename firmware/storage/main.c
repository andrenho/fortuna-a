#include <stdio.h>
#include <stdint.h>

#include <avr/cpufunc.h>
#include <avr/io.h>
#include <util/delay.h>

#include "uart.h"

#define get_Y2W() (PINB & _BV(PB1))
#define get_Y2R() (PINB & _BV(PB0))

#define get_DATA()     (PIND)
#define set_DATA(v)    { DDRD = 0xff; PORTD = v; }
#define release_DATA() { PORTD = 0x0; DDRD = 0x0; }

#define set_LED(v) { if (v) PORTC |= _BV(PC0); else PORTC &= ~_BV(PC0); }
#define set_R(v)   { if (v) PORTC |= _BV(PC1); else PORTC &= ~_BV(PC1); }

// #define pulse_R() { PORTC |= _BV(PC1); _delay_us(200); PORTC &= ~_BV(PC1); _delay_us(200); }
#define pulse_R() { PORTC &= ~_BV(PC1); _delay_us(200); PORTC |= _BV(PC1); _delay_us(200); }

int main()
{
    DDRC |= _BV(PC0) | _BV(PC1);
    DDRD = 0x0;

    PORTC = 0b11;

    pulse_R();

    for (;;) {
        if (get_Y2W() == 0) {
            uint8_t data = get_DATA();
            set_LED(data == 0x24);
            pulse_R();
        } else if (get_Y2R() == 0) {
            set_DATA(0x65);
            _delay_us(200);
            pulse_R();
            release_DATA();
            while (get_Y2R() == 0);
        }
    }

    /*
    uart_init();

    for (;;) {
        printf("Hello.\n");
        PORTC |= _BV(PC0);
        _delay_ms(200);
        PORTC &= ~_BV(PC0);
        _delay_ms(200);
    }
    */
}
