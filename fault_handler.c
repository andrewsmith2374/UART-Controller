#include <stdlib.h>
#include "controller.h"

void *fault_handler(int fd) {

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
            handle_fault();
        } else if (ret < 0) {
            perror("signal");
            exit(-1);
        } else {
            clear_fault();
        }
    }
}
