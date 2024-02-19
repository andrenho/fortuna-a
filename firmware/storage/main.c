#include <stdio.h>
#include <stdint.h>

#include <avr/io.h>
#include <util/delay.h>

#include "uart.h"

#define get_Y2W() (PINB & _BV(PB1))
#define set_R(v)   { if (v) PORTC |= _BV(PC1); else PORTC &= ~_BV(PC1); }
#define set_LED(v) { if (v) PORTC |= _BV(PC0); else PORTC &= ~_BV(PC0); }
#define get_DATA() (PIND)

int main()
{
    DDRC |= _BV(PC0) | _BV(PC1);
    PORTC = 0x0;

    set_R(1);
    set_R(0);

    for (;;) {
        if (get_Y2W() == 0) {
            uint8_t data = get_DATA();
            set_LED(data == 0x3e);
            _delay_us(200);
            set_R(1);
            set_R(0);
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
