#include <stdio.h>
#include <stdint.h>

#include <avr/io.h>
#include <util/delay.h>

#include "uart.h"

int main()
{
    DDRC |= _BV(PC0) | _BV(PC1);
    PORTC = 0x0;

    uart_init();

    for (;;) {
        printf("Hello.\n");
        PORTC |= _BV(PC0);
        _delay_ms(200);
        PORTC &= ~_BV(PC0);
        _delay_ms(200);
    }
}
