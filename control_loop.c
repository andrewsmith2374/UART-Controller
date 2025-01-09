#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/select.h>
#include <unistd.h>
#include "controller.h"

/* Monitor for incoming temperature data.
 * Upon receiving data, adjust fan speed accordingly.
 * Log fan and temperature data each second.
 */
void *control_loop(int fd, uart_t *sensor, uart_t *remote) {

    int temperature, curr_fan_speed;
    int BUF_SIZE = 1024;
    char buf[BUF_SIZE];

    struct timeval timeout;
    int ret;

    // Set timeout values
    timeout.tv_sec = LOGGING_INTERVAL;
    timeout.tv_usec = 0;

    curr_fan_speed = 0;
    while (1) { // Read from pipe

        // Initialize set
        fd_set set;
        FD_ZERO(&set);
        FD_SET(fd, &set);

        ret = select(fd + 1, &set, NULL, NULL, &timeout);
        printf("Data received\n");
        if (ret == 0) {
            sprintf(buf, "Temperature: %d\nFan Speed: %d\n", temperature, curr_fan_speed);
            printf("%s", buf);
            if (uart_send(remote, buf, strnlen(buf, BUF_SIZE)) < 0) {
                perror("uart send");
                exit(-1);
            }
        } else if (ret < 0) {
            perror("signal");
            exit(-1);
        } else {
            // printf("fd: %d\n", fd);
            temperature = read_temp_data(fd);
            if (temperature > 50) {
                curr_fan_speed = FAN_SPEED;
            } else if (temperature < 40) {
                curr_fan_speed = 0;
            }
            set_fan_speed(curr_fan_speed);
        }
    }
    return NULL;

}

/* Read temperature data from given file descriptor. */
int read_temp_data(int fd) {
    long buf[1];
    int nbytes;

    // printf("trying to read\n");
    
    nbytes = read(fd, buf, sizeof(buf) - 1);
    if (nbytes < sizeof(long)) {
        perror("read");
        exit(-1);
    }
    return *buf; // strtol(buf, NULL, 10);
}

/* Set fan speed to given value over PWM. */
int set_fan_speed(uint8_t speed) {
    printf("Set fan speed to %d\n", speed);
    return 0;
}
