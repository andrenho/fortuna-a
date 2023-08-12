#include <stdio.h>

#include <avr/cpufunc.h>

#include "buffer.hh"
#include "bus.hh"
#include "comm.hh"
#include "uart.hh"
#include "random.hh"

int main(void)
{
    random::init();
    bus::init();
    uart_init();

    bus::set_rst(1);
    // _NOP();
    // bus::set_rst(0);

    printf("Start.\n");  // TODO

    for (;;)
        comm::listen();

    return 0;
}
