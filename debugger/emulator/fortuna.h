#ifndef EMULATOR_FORTUNA_H
#define EMULATOR_FORTUNA_H

#include <stddef.h>
#include <stdint.h>

void fortuna_init();
void fortuna_reset();
void fortuna_write_to_rom(uint16_t addr, int* values, size_t n_values);

#endif //EMULATOR_FORTUNA_H
