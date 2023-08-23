#include <stdlib.h>
#include <stdio.h>
#include "comm.h"
#include "fortuna.h"

#include "z80/Z80.h"

int main()
{
    char serial_port[100];
    if (!comm_init(serial_port))
        exit(EXIT_FAILURE);

    printf("Listening on serial port: %s\n", serial_port);

    fortuna_init();

    // read serial port
    while (1) {
        char buffer[1024];
        comm_readline(buffer, sizeof buffer);

        comm_printf("%s\n", buffer);  // echo

        char cmd;
        int values[512];
        size_t n_values = comm_scanf(buffer, &cmd, values, sizeof values);

        switch (cmd) {
            case 'A':
                comm_printf("+\n");
                break;
            case 'X':
                fortuna_reset();
                comm_printf("+\n");
                break;
            case 'R':
                comm_printf("+ %x ", values[1]);
                for (int i = values[0]; i < (values[0] + values[1]); ++i)
                    comm_printf("%x ", RdZ80(i));
                comm_printf("\n");
                break;
            case 'W':
                fortuna_write_to_rom(values, n_values);
                comm_printf("+\n");
                break;
            default:
                comm_printf("x\n");
        }
    }

    return 0;
}