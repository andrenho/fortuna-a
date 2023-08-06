#include "bus.hh"

#include <avr/io.h>
#include <avr/cpufunc.h>

#define SET_PIN(port, pin, v) { if (v) port |= _BV(pin); else port &= ~_BV(pin); }

namespace bus {

void init()
{
    DDRA = 0b00011111;   // rom_we, busrq, nmi, clk, clk_ena
    DDRJ = 0b00000010;   // rst
    
    set_rom_we(1);
    set_busrq(1);
    set_nmi(1);
    set_clk_ena(false);  // clock is controlled by debugger
    set_rst(0);          // put Z80 in reset mode
    
    PORTA &= ~_BV(PA3);  // clock initial position = 0
}

void set_rom_we(bool v) { SET_PIN(PORTA, PA0, v) }

void set_nmi(bool v) { SET_PIN(PORTA, PA2, v) }

void set_clk_ena(bool v) { SET_PIN(PORTA, PA4, v) }

void set_busrq(bool v) { SET_PIN(PORTA, PA1, v) }

void set_rst(bool v) { SET_PIN(PORTJ, PJ1, v) }

void clk_pulse()
{
    PORTA |= _BV(PA3);
    _NOP();
    PORTA &= ~_BV(PA3);
}

MemPins get_mem()
{
    uint8_t a = PINA;
    return {
        (bool) (a & _BV(5)),
        (bool) (a & _BV(6)),
        (bool) (a & _BV(7))
    };
}

void set_mem(MemPins mem)
{
    DDRA |= 0b11100000;
    SET_PIN(PORTA, PA5, mem.wr);
    SET_PIN(PORTA, PA6, mem.rd);
    SET_PIN(PORTA, PA7, mem.mreq);
}

void release_mem()
{
    DDRA &= ~0b11100000;
}

void set_y0w(bool v)
{
    DDRJ |= 0b1;
    SET_PIN(PORTJ, PJ0, v);
}

void release_y0w()
{
    DDRJ &= (uint8_t) ~0b1;
}

uint8_t get_data()
{
    return PINC;
}

void set_data(uint8_t data)
{
    DDRC = 0xff;
    PORTC = data;
}

void release_data()
{
    DDRC = 0;
}

uint16_t get_addr()
{
    return (((uint16_t) PINK) << 8) | PINF;
}

void set_addr(uint16_t addr)
{
    DDRK = 0xff;
    DDRF = 0xff;
    PORTK = addr >> 8;
    PORTF = addr & 0xff;
}

void release_addr()
{
    DDRK = 0;
    DDRF = 0;
}

uint8_t get_addr_high()
{
    uint8_t b = PINB;
    return (b >> 4) & 0b111;
}

bool get_ramonly()
{
    return PIND & 1;
}

}
