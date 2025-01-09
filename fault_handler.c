#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "controller.h"

#define MSG_SIZE 256

/* Monitor for communication.
 * Raise communication error upon timeout.
 * Upon receiving a signal, clear the error.
 */
void *fault_handler(int fd, uart_t *remote) {

    int fault = 0;
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
        if (ret == 0 && !fault) { // Timeout occurred
            handle_fault(remote);
            fault = 1;
        } else if (ret < 0) {
            perror("signal");
            exit(-1);
        } else if (fault) {
            clear_fault(remote);
            fault = 0;
        }
    }
}

/* Log a fault to UART. */
void handle_fault(uart_t *remote) {
    char msg[MSG_SIZE];
    sprintf(msg, "%ld: communication fault", time(NULL));
    printf("%s\n", msg);
    if (uart_send(remote, msg, MSG_SIZE) < 0) {
        perror("uart send");
        exit(-1);
    }
}

/* Clear faults from UART. */
void clear_fault(uart_t *remote) {
    char msg[256];
    sprintf(msg, "%ld: communication fault cleared", time(NULL));
    printf("%s\n", msg);
    if (uart_send(remote, msg, MSG_SIZE) < 0) {
        perror("uart send");
        exit(-1);
    }
}
