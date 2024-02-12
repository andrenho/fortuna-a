#include <stdio.h>
#include <stdint.h>

#include <avr/io.h>

#include "uart.h"

int main()
{
    DDRC |= _BV(PC4);

    uart_init();
    PORTC |= _BV(PC4);

    for (;;) {
        printf("Hello.\n");
    }
}
