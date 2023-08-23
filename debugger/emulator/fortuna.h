#ifndef EMULATOR_FORTUNA_H
#define EMULATOR_FORTUNA_H

#include <stddef.h>

void fortuna_init();
void fortuna_reset();
void fortuna_write_to_rom(int* values, size_t n_values);

#endif //EMULATOR_FORTUNA_H
