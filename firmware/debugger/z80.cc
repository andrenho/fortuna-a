#include "z80.hh"

#include <stddef.h>

namespace z80 {

bool is_present()
{
    return true;  // TODO
}

void release_bus()
{
    if (bus::get_busak() == 0)
        return;

    bus::set_busrq(0);
    bus::pulse_clk();
    while (bus::get_busak() != 0)
        bus::pulse_clk();
    bus::set_busrq(1);
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

StepCycleStatus step_cycle()
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

uint16_t step(bool nmi)
{
    bus::set_busrq(1);  // make sure we're not requesting the bus
    bus::pulse_clk();

    while (bus::get_m1() != 0)
        bus::pulse_clk();

    bus::pulse_clk();

    static uint8_t previous_instruction = 0x00;
    bool combined_instruction = (previous_instruction == 0xcb || previous_instruction == 0xdd || previous_instruction == 0xed || previous_instruction == 0xfd);
    previous_instruction = bus::get_data();
    if (combined_instruction)
        step(nmi);

    // TODO - NMI

    return bus::get_addr();
}

static bool is_breakpoint(uint16_t addr)
{
    return false;
}

uint16_t debug_run() {
    StepStatus ss;

    do {
        ss = step(false);
    } while (!is_breakpoint(ss.pc));

    return ss.pc;
}

}
