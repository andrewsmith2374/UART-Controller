#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "controller.h"

#define DEV_NAME "S0"
#define BAUD_RATE 9600
#define OPT "8N1N"

int main(void) {
    pthread_t fault_thread, control_thread;
    int ffds[2], p; // For signalling to fault thread
    
    uart_t *remote_sensor = uart_open(DEV_NAME, BAUD_RATE, OPT);
    if (remote_sensor == NULL) {
        perror("failed to connect to sensor");
        exit(-1);
    }

    if (pipe(ffds) == -1) {
        perror("pipe failed");
        exit(-1);
    }

    p = fork();
    if (p > 0) { // Parent

        close(ffds[0]);
        // TODO: Fork off control loop

        // Main Loop
        while(1) {
            if (uart_bytes_get(remote_sensor) > 0) { // Data available
                writefd(ffds[1], DATA_REC, sizeof(int));
            }
        }

    } else if (p == 0) { // Child
        close(ffds[1]);
        fault_handler();
    } else {
        perror("fork failed");
        exit(-1);
    }

    // pthread_create(&fault_thread, NULL, &fault_handler, NULL);
    // pthread_create(&control_thread, NULL, &control_loop, (void *) remote_sensor);
}
