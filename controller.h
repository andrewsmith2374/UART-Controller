#include "libuart-0.2.0.1/uart.h"
#include "libuart-0.2.0.1/error.h"
#include "utils.c"

#define DATA_REC "1"
#define TIMEOUT 2
#define MAX_TEMP 150
#define MIN_TEMP 0

void *fault_handler(int fd);
void *control_loop(int fd, uart_t *sensor);

void main_loop(uart_t *remote_sensor, int fault_fd, int control_fd);
