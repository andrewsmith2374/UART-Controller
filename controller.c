#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "controller.h"

#define DEV_NAME "S0"
#define BAUD_RATE 9600
#define OPT "8N1N"

int main(void) {
    int fault_fds[2], control_fds[2], p; // For signalling to fault thread

    // Initialize sensor
    uart_t *remote_sensor = uart_open(DEV_NAME, BAUD_RATE, OPT);
    if (remote_sensor == NULL) {
        perror("failed to connect to sensor");
        exit(-1);
    }

    // Set up fault child pipe
    if (pipe(fault_fds) == -1) {
        perror("pipe failed");
        exit(-1);
    }

    p = fork();
    if (p > 0) { // Parent

        close(fault_fds[0]);

        // Set up control child pipe
        if (pipe(control_fds) == -1) {
            perror("pipe failed");
            exit(-1);
        }

        p = fork();
        if (p > 0) { // Parent again
            close(control_fds[0]);
            main_loop(remote_sensor, fault_fds[1], control_fds[1]);
        } else if (p == 0) { // Child
            close(fault_fds[1]);
            close(control_fds[1]);
            control_loop(control_fds[1], remote_sensor);
        }
    } else if (p == 0) { // Child
        close(fault_fds[1]);
        fault_handler(fault_fds[0]);
    } else {
        perror("fork failed");
        exit(-1);
    }
}

void main_loop(uart_t *remote_sensor, int fault_fd, int control_fd) {
    char *buf = NULL;
    long temp;

    while (1) { // Busy waiting but library doesn't provide anything better
        if (uart_bytes_get(remote_sensor) > 0) { // Data available
            uart_recv(remote_sensor, buf, sizeof(int));
            temp = strtol(buf, NULL, 10);
            if (temp < MIN_TEMP || temp > MAX_TEMP) {
                perror("invalid temperature reading");
                continue;
            }

            writefd(fault_fd, DATA_REC, sizeof(char *));
            writefd(control_fd, buf, sizeof(char *));
        }
    }
}
