#include "libuart-0.2.0.1/uart.h"
#include "libuart-0.2.0.1/error.h"
#include "utils.c"

#define DATA_REC "1"

void *fault_handler();
void *control_loop(uart_t *sensor);
