#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/select.h>
#include <unistd.h>
#include "controller.h"

void *control_loop(int fd, uart_t *sensor, uart_t *remote) {

    int temperature, curr_fan_speed;
    int BUF_SIZE = 1024;
    char buf[BUF_SIZE];

    struct timeval timeout;
    int ret;
    fd_set set;

    // Initialize set
    FD_ZERO(&set);
    FD_SET(fd, &set);

    // Set timeout values
    timeout.tv_sec = LOGGING_INTERVAL;
    timeout.tv_usec = 0;

    curr_fan_speed = 0;
    while (1) { // Read from pipe

        ret = select(fd + 1, &set, NULL, NULL, &timeout);
        if (ret == 0) {
            // Log to UART
            sprintf(buf, "Temperature: %d\nFan Speed: %d\n", temperature, curr_fan_speed);
            log_message(remote, buf, strnlen(buf, BUF_SIZE));
        } else if (ret < 0) {
            perror("signal");
            exit(-1);
        } else {
            temperature = read_temp_data(fd);
            if (temperature > 50) {
                curr_fan_speed = 0;
            } else if (temperature < 40) {
                curr_fan_speed = FAN_SPEED;
            }
            set_fan_speed(curr_fan_speed);
        }
    }

    return NULL;
}

int read_temp_data(int fd) {
    char buf[1];
    int nbytes;
    
    nbytes = read(fd, buf, 1);
    if (nbytes < 1) {
        perror("read");
        exit(-1);
    }
    return strtol(buf, NULL, 10);
}

int set_fan_speed(uint8_t speed) { return -1; }
