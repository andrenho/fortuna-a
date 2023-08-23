#ifndef EMULATOR_COMM_H
#define EMULATOR_COMM_H

#include <stdbool.h>
#include <stddef.h>

bool comm_init(char* serial_port);
void comm_readline(char* line, size_t max_size);
void comm_printf(const char* fmt, ...);

#endif //EMULATOR_COMM_H
