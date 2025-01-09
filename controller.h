#include <stdint.h>

#include "libuart-0.2.0.1/uart.h"
#include "libuart-0.2.0.1/error.h"

#define DATA_REC "1"
#define TIMEOUT 2
#define LOGGING_INTERVAL 1
#define MAX_TEMP 150
#define MIN_TEMP 0
#define FAN_SPEED 255

void *fault_handler(int fd, uart_t *remote);
void *control_loop(int fd, uart_t *sensor, uart_t *remote);

void main_loop(uart_t *remote_sensor, int fault_fd, int control_fd);
void test_loop(int fault_fd, int control_fd);
void writefd(int fd, char *message, int len);

void handle_fault(uart_t *remote);
void clear_fault(uart_t *remote);

int read_temp_data(int fd);
int set_fan_speed(uint8_t speed);
