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

    bus::set_rst(1);

    const uint16_t ADDR = 0x1;

    printf("Start:\n");

    printf("%d\n", post::read_rom_memory());
    printf("%d\n", post::read_shared_memory());
    printf("%d\n", post::read_high_memory());
    printf("%d\n", post::write_rom_memory());
    printf("%s\n", post::error);
    printf("%d\n", post::write_shared_memory());
    printf("%d\n", post::write_high_memory());

    random::write_to_eeprom();

    printf("End.\n");

    for(;;);

    return 0;
}
