#include <stdlib.h>
#include <sys/select.h>
#include "controller.h"

void *control_loop(int fd, uart_t *sensor) {

    struct timeval timeout;
    int ret;
    char inbuf[1];
    fd_set set;

    // Initialize set
    FD_ZERO(&set);
    FD_SET(fd, &set);

    // Set timeout values
    timeout.tv_sec = TIMEOUT;
    timeout.tv_usec = 0; /* microseconds */

    while (1) { // Read from pipe

        ret = select(fd + 1, &set, NULL, NULL, &timeout);

        if (ret == 0) {
            // Timeout occurred
        } else if (ret < 0) {
            perror("signal");
            exit(-1);
        } else {
            // Read in temperature data
        }
    }

    return NULL;
}
