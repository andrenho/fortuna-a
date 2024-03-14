#include "output.h"

#include <avr/io.h>

#include "spi.h"

void output_init()
{
    DDRB |= _BV(DDB0);  // output: RCLK
}

void output_set(uint8_t data)
{
    spi_send(data);

    // RSCK clock
    PORTB |= _BV(PB0);
    PORTB &= ~_BV(PB0);
}
