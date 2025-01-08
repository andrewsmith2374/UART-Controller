#include <stdlib.h>
#include "libuart-0.2.0.1/uart.h"

#define DEV_NAME "S0"
#define BAUD_RATE 9600
#define OPT "8N1N"

int main(void) {
    
    uart_t *remote_sensor = uart_open(DEV_NAME, BAUD_RATE, OPT);
    if (remote_sensor == NULL) { return -1; }
}
