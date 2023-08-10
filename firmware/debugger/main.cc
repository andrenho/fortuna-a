#include <stdio.h>

#include <avr/pgmspace.h>

#include "bus.hh"
#include "memory.hh"
#include "uart.hh"

int main(void)
{
    bus::init();
    uart_init();

    // bus::set_rst(1);  // TODO - this will not work when we have the CPU
    // memory::set_memory_state(0, true);

    for (size_t i = 0; i < 32; ++i)
        memory::set(0x9000, i);

    for (size_t i = 0; i < 32; ++i)
        printf("%02X ", memory::get(0x9000 + i));
    printf("\n");
    
    for(;;);

    return 0;
}
