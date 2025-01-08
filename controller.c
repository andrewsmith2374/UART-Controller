#include <stdlib.h>
#include <pthread.h>
#include "controller.h"

#define DEV_NAME "S0"
#define BAUD_RATE 9600
#define OPT "8N1N"

int main(void) {
    pthread_t fault_thread, control_thread;
    
    uart_t *remote_sensor = uart_open(DEV_NAME, BAUD_RATE, OPT);
    if (remote_sensor == NULL) { return -1; }

    pthread_create(&fault_thread, NULL, &fault_handler, NULL);
    pthread_create(&control_thread, NULL, &control_loop, NULL);

    // Main Loop
}
