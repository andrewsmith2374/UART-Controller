#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include "controller.h"

/* Write to a given file descriptor. */
void writefd(int fd, char *message, int len) {
    ssize_t ret;
    while(len != 0 && (ret = write(fd, message, len)) != 0) {
        if(ret == -1) {
            if(errno == EINTR) { continue; }
            perror("write");
            exit(-1);
        }
        len -= ret;
        message += ret;
    }
}

void log_message(uart_t *uart, char *msg, size_t size) {
    return;
}
