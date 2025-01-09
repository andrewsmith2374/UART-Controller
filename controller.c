#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "controller.h"

#define SENSOR_NAME "S0"
#define REMOTE_NAME "R0"
#define BAUD_RATE 9600
#define OPT "8N1N"

int main(void) {
    int fault_fds[2], control_fds[2], p; // For signalling to fault thread

    // Initialize sensor
    uart_t *remote_sensor = uart_open(SENSOR_NAME, BAUD_RATE, OPT);
    if (remote_sensor == NULL) {
        perror("failed to connect to sensor");
        exit(-1);
    }

    // Initialize logging UART
    uart_t *logging  = uart_open(REMOTE_NAME, BAUD_RATE, OPT);
    if (logging == NULL) {
        perror("failed to connect to logging UART");
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
            test_loop(fault_fds[1], control_fds[1]);
        } else if (p == 0) { // Child
            close(fault_fds[1]);
            close(control_fds[1]);
            control_loop(control_fds[1], remote_sensor, logging);
        }
    } else if (p == 0) { // Child
        close(fault_fds[1]);
        fault_handler(fault_fds[0], logging);
    } else {
        perror("fork failed");
        exit(-1);
    }
}

/* Continuously check for communication on remote sensor.
 * If data available, validate and send data to control loop and fault handler.
 */
void main_loop(uart_t *remote_sensor, int fault_fd, int control_fd) {
    char *buf = NULL;
    long temp;

    struct timespec sleep_interval;
    sleep_interval.tv_sec = 0;
    sleep_interval.tv_nsec = 100 * 1000000; // Nanoseconds (100 ms)

    while (1) { // Busy waiting but library doesn't provide anything better
        if (uart_bytes_get(remote_sensor) > 0) { // Data available
            uart_recv(remote_sensor, buf, sizeof(int));
            temp = strtol(buf, NULL, 10);
            if (temp < MIN_TEMP || temp > MAX_TEMP) { // Validate temperature
                perror("invalid temperature reading");
                continue;
            }

            writefd(fault_fd, DATA_REC, sizeof(char *));
            writefd(control_fd, buf, sizeof(char *));
        }
        // Sleep for 100ms to reduce overhead while maintaining timing precision
        nanosleep(&sleep_interval, NULL);
    }
}

void test_loop(int fault_fd, int control_fd) {

    char *buf = "25";
    long temp;

    struct timespec sleep_interval;
    sleep_interval.tv_sec = 1;
    sleep_interval.tv_nsec = 0;

    while (1) { // Busy waiting but library doesn't provide anything better

        writefd(fault_fd, DATA_REC, sizeof(char *));
        writefd(control_fd, buf, sizeof(char *));

        // Sleep for 100ms to reduce overhead while maintaining timing precision
        nanosleep(&sleep_interval, NULL);
    }
}
