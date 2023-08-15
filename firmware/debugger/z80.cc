#include "z80.hh"

#include <stddef.h>

namespace z80 {

bool is_present()
{
    return false;  // TODO
}

void release_bus()
{
    // TODO
}

void reset()
{
    bus::set_rst(0);
    for (size_t i = 0; i < 50; ++i)
        bus::pulse_clk();
    bus::set_rst(1);
}

void clk()
{
    bus::pulse_clk();
}

StepStatus step()
{
    bus::pulse_clk();
    return {
        bus::get_data(),
        bus::get_addr(),
        bus::get_m1(),
        bus::get_iorq(),
        bus::get_busak(),
        bus::get_wait(),
        bus::get_int(),
        bus::get_mem()
    };
}

}
