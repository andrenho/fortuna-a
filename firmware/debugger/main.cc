#include <stdio.h>

#include <avr/pgmspace.h>

#include "bus.hh"
#include "memory.hh"
#include "uart.hh"

int main(void)
{
    bus::init();
    uart_init();

    memory::set(0, 0);
    
    for(;;);

    return 0;
}
