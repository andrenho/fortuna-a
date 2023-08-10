#include <stdio.h>

#include <avr/pgmspace.h>

#include "bus.hh"
#include "memory.hh"
#include "uart.hh"
#include "post.hh"
#include "random.hh"

int main(void)
{
    random::init();
    bus::init();
    uart_init();

    printf("Start:\n");
    printf("%d\n", post::read_rom_memory());
    printf("%d\n", post::read_shared_memory());
    printf("%d\n", post::read_high_memory());
    printf("%d\n", post::write_rom_memory());
    printf("%s\n", post::error);
    printf("%d\n", post::write_shared_memory());
    printf("%d\n", post::write_high_memory());

    random::write_to_eeprom();

    for(;;);

    return 0;
}
