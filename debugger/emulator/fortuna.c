#include "fortuna.h"

#include <stdint.h>

#include "z80/Z80.h"

static uint8_t ram[512 * 1024];
static Z80 z80;

void fortuna_init()
{
}

void fortuna_reset()
{
    ResetZ80(&z80);
}

void fortuna_write_to_rom(uint16_t addr, int* values, size_t n_values)
{
    for (size_t i = addr; i < addr + n_values; ++i)
        ram[i] = (uint8_t) values[i];
}

byte InZ80(register word Port)
{
    return 0;
}

void OutZ80(register word Port,register byte Value)
{
}

void PatchZ80(register Z80 *R)
{
}

word LoopZ80(register Z80 *R)
{
    return INT_QUIT;
}

void WrZ80(register word Addr,register byte Value)
{
    ram[Addr] = Value;  // TODO - memory map + block access to BIOS
}

byte RdZ80(register word Addr)
{
    return ram[Addr];  // TODO - memory map
}