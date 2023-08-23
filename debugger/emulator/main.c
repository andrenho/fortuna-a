#include <stdlib.h>
#include <stdio.h>
#include "comm.h"

int main()
{
    char serial_port[100];
    if (!comm_init(serial_port))
        exit(EXIT_FAILURE);

    printf("Listening on serial port: %s\n", serial_port);

    // read serial port
    while (1) {
        char buffer[1024];
        comm_readline(buffer, sizeof buffer);

        switch (buffer[0]) {
            case 'A':
                comm_printf("+\r\n");
                break;
        }
    }

    return 0;
}