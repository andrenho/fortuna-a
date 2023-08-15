#include <stdio.h>

#include <avr/cpufunc.h>

#include "buffer.hh"
#include "bus.hh"
#include "comm.hh"
#include "uart.hh"
#include "random.hh"
#include "z80.hh"

int main()
{
    random::init();
    bus::init();
    uart_init();

    z80::reset();
    // _NOP();
    // bus::set_rst(0);

    // printf("Start.\n");  // TODO

    for (;;)
        comm::listen();

    return 0;
}
