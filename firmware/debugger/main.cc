#include <stdio.h>

#include <avr/pgmspace.h>

#include "bus.hh"
#include "memory.hh"
#include "uart.hh"

int main(void)
{
    bus::init();
    uart_init();

    bus::set_rst(1);  // TODO - this will not work when we have the CPU
    memory::set_memory_state(0, true);

    memory::set(0x1000, 0x0);
    
    for(;;);

    return 0;
}
