#include <stdio.h>

#include <avr/pgmspace.h>

#include "bus.hh"
#include "memory.hh"
#include "uart.hh"

int main(void)
{
    bus::init();
    uart_init();

    memory::get(0x1000);
    
    for(;;);

    return 0;
}
