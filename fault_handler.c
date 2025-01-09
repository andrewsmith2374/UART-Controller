#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "controller.h"

#define MSG_SIZE 256

void *fault_handler(int fd, uart_t *remote) {

    struct timeval timeout;
    int ret;
    fd_set set;

    // Initialize set
    FD_ZERO(&set);
    FD_SET(fd, &set);

    // Set timeout values
    timeout.tv_sec = TIMEOUT;
    timeout.tv_usec = 0; /* microseconds */

    while (1) { // Read from pipe

        ret = select(fd + 1, &set, NULL, NULL, &timeout);
        if (ret == 0) { // Timeout occurred
            handle_fault(remote);
        } else if (ret < 0) {
            perror("signal");
            exit(-1);
        } else {
            clear_fault(remote);
        }
    }
}

void handle_fault(uart_t *remote) {
    char msg[MSG_SIZE];
    sprintf(msg, "%ld: communication fault", time(NULL));
    uart_send(remote, msg, MSG_SIZE);
}

void clear_fault(uart_t *remote) {
    char msg[256];
    sprintf(msg, "%ld: communication fault cleared", time(NULL));
    uart_send(remote, msg, MSG_SIZE);
}
