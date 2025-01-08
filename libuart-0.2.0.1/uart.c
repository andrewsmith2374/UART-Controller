/**
 *
 * File Name: uart.c
 * Title    : UART library source
 * Project  : libuart
 * Author   : Copyright (C) 2018 Johannes Krottmayer <krjdev@gmail.com>
 * Created  : 2018-05-21
 * Modified : 2018-12-25
 * Revised  : 
 * Version  : 0.2.0.1
 * License  : ISC (see file LICENSE.txt)
 *
 * NOTE: This code is currently below version 1.0, and therefore is considered
 * to be lacking in some functionality or documentation, or may not be fully
 * tested. Nonetheless, you can expect most functions to work.
 *
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __unix__
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#elif _WIN32
#include <Windows.h>
#endif /* __unix__ or _WIN32 */

#include "uart.h"
#include "error.h"

static int enum_contains(int enum_values[], int len, int value)
{
    int i;
    
    for (i = 0; i < len; i++)
        if (enum_values[i] == value)
            return 1;
    
    return 0;
}

static int is_baud_valid(int value)
{
    int E[] = {
#ifdef __unix__
        UART_BAUD_0,
        UART_BAUD_50,
        UART_BAUD_75,
#endif
        UART_BAUD_110,
#ifdef __unix__
        UART_BAUD_134,
        UART_BAUD_150,
        UART_BAUD_200,
#endif
        UART_BAUD_300,
        UART_BAUD_600,
        UART_BAUD_1200,
#ifdef __unix__
        UART_BAUD_1800,
#endif
        UART_BAUD_2400,
        UART_BAUD_4800,
        UART_BAUD_9600,
#ifdef _WIN32
        UART_BAUD_14400,
#endif
        UART_BAUD_19200,
        UART_BAUD_38400,
        UART_BAUD_57600,
        UART_BAUD_115200,
#ifdef _WIN32
        UART_BAUD_128000,
        UART_BAUD_256000
#elif __unix__
        UART_BAUD_230400,
        UART_BAUD_460800,
        UART_BAUD_500000,
        UART_BAUD_576000,
        UART_BAUD_921600,
        UART_BAUD_1000000,
        UART_BAUD_1152000,
        UART_BAUD_1500000,
        UART_BAUD_2000000,
        UART_BAUD_2500000,
        UART_BAUD_3000000,
        UART_BAUD_3500000,
        UART_BAUD_4000000
#endif /* __unix__ or _WIN32 */
    };
    
    if (enum_contains(E, sizeof(E)/sizeof(E[0]), value))
        return 1;
    
    return 0;
}

static int is_databits_valid(int value)
{
    if ((value > 4) && (value < 9))
        return 1;
    
    return 0;
}

static int is_parity_valid(int value)
{
    int ret = 0;
    
    switch (value) {
    case UART_PARITY_NO:
        ret = 1;
        break;
    case UART_PARITY_ODD:
        ret = 1;
        break;
    case UART_PARITY_EVEN:
        ret = 1;
        break;
    default:
        break;
    }
    
    return ret;
}

static int is_stopbits_valid(int value)
{
    if ((value > 0) && (value < 3))
        return 1;
    
    return 0;
}

static int is_flow_valid(int value)
{
    int ret = 0;
    
    switch (value) {
    case UART_FLOW_NO:
        ret = 1;
        break;
    case UART_FLOW_SOFTWARE:
        ret = 1;
        break;
    case UART_FLOW_HARDWARE:
        ret = 1;
        break;
    default:
        break;
    }
    
    return ret;
}

static int parse_option(uart_t *uart, const char *opt)
{
    int i = 0;
    
    if (!uart) {
        printerr_uart_type_invalid();
        return -1;
    }
    
    if (!opt) {
        printerr_string_null();
        return -1;
    }
    
    while (opt[i] != '\0') {
        /* parse data bits */
        switch (opt[i]) {
        case '5':
            uart->data_bits = 5;
            break;
        case '6':
            uart->data_bits = 6;
            break;
        case '7':
            uart->data_bits = 7;
            break;
        case '8':
            uart->data_bits = 8;
            break;
        default:
            printerr_uart_opt_invalid("data bits");
            return -1;
        }
        
        i++;
        
        /* parse parity */
        switch (opt[i]) {
        case 'N':
            uart->parity = UART_PARITY_NO;
            break;
        case 'O':
            uart->parity = UART_PARITY_ODD;
            break;
        case 'E':
            uart->parity = UART_PARITY_EVEN;
            break;
        default:
            printerr_uart_opt_invalid("parity");
            return -1;
        }
        
        i++;
        
        /* parse stop bits */
        switch (opt[i]) {
        case '1':
            uart->stop_bits = 1;
            break;
        case '2':
            uart->stop_bits = 2;
            break;
        default:
            printerr_uart_opt_invalid("stop bits");
            return -1;
        }
        
        i++;
        
        /* parse flow control */
        switch (opt[i]) {
        case 'N':
            uart->flow_ctrl = UART_FLOW_NO;
            break;
        case 'S':
            uart->flow_ctrl = UART_FLOW_SOFTWARE;
            break;
        case 'H':
            uart->flow_ctrl = UART_FLOW_HARDWARE;
            break;
        default:
            printerr_uart_opt_invalid("flow control");
            return -1;
        }
        
        i++;
        
        if (opt[i] != '\0') {
            printerr_uart_opt_invalid(NULL);
            return -1;
        }
    }
    
    return 0;
}

static int init_baud(uart_t *uart)
{
    int ret;
#ifdef __unix__
    struct termios options;
#elif _WIN32
    DCB dcb = { 0 };
    dcb.DCBlength = sizeof(dcb);
    DWORD dwerror;
    LPVOID lpmessage;
#endif /* __unix__ or _WIN32 */
    
#ifdef __unix__
    ret = tcgetattr(uart->fd, &options);
    
    if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
    }
#elif _WIN32
    ret = GetCommState(uart->h, &dcb);
    
    if (!ret) {
        dwerror = GetLastError();
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                  FORMAT_MESSAGE_IGNORE_INSERTS, 
                  NULL, 
                  dwerror, 
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
                  (LPTSTR) &lpmessage, 
                  0, 
                  NULL);
        printerr_uart_comm((const char *) lpmessage);
        LocalFree(lpmessage);
        return -1;
    }
#endif /* __unix__ or _WIN32 */
    
    switch (uart->baud) {
#ifdef __unix__
    case UART_BAUD_0:
        ret = cfsetispeed(&options, B0);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        ret = cfsetospeed(&options, B0);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }

        break;
    case UART_BAUD_50:
        ret = cfsetispeed(&options, B50);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        ret = cfsetospeed(&options, B50);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }

        break;
    case UART_BAUD_75:
        ret = cfsetispeed(&options, B75);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        ret = cfsetospeed(&options, B75);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }

        break;
#endif /* __unix__ */
    case UART_BAUD_110:
#ifdef __unix__
        ret = cfsetispeed(&options, B110);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        ret = cfsetospeed(&options, B110);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
#elif _WIN32
        dcb.BaudRate = CBR_110;
#endif /* __unix__ or _WIN32 */
        break;
#ifdef __unix__
    case UART_BAUD_134:
        ret = cfsetispeed(&options, B134);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        ret = cfsetospeed(&options, B134);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }

        break;
    case UART_BAUD_150:
        ret = cfsetispeed(&options, B150);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        ret = cfsetospeed(&options, B150);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }

        break;
    case UART_BAUD_200:
        ret = cfsetispeed(&options, B200);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        ret = cfsetospeed(&options, B200);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }

        break;
#endif /* __unix__ */
    case UART_BAUD_300:
#ifdef __unix__
        ret = cfsetispeed(&options, B300);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        ret = cfsetospeed(&options, B300);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
#elif _WIN32
        dcb.BaudRate = CBR_300;
#endif /* __unix__ or _WIN32 */
        break;
    case UART_BAUD_600:
#ifdef __unix__
        ret = cfsetispeed(&options, B600);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        ret = cfsetospeed(&options, B600);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
#elif _WIN32
        dcb.BaudRate = CBR_600;
#endif /* __unix__ or _WIN32 */
        break;
    case UART_BAUD_1200:
#ifdef __unix__
        ret = cfsetispeed(&options, B1200);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        ret = cfsetospeed(&options, B1200);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
#elif _WIN32
        dcb.BaudRate = CBR_1200;
#endif /* __unix__ or _WIN32 */
        break;
#ifdef __unix__
    case UART_BAUD_1800:
        ret = cfsetispeed(&options, B1800);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        ret = cfsetospeed(&options, B1800);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }

        break;
#endif /* __unix__ */
    case UART_BAUD_2400:
#ifdef __unix__
        ret = cfsetispeed(&options, B2400);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        ret = cfsetospeed(&options, B2400);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
#elif _WIN32
        dcb.BaudRate = CBR_2400;
#endif /* __unix__ or _WIN32 */
        break;
    case UART_BAUD_4800:
#ifdef __unix__
        ret = cfsetispeed(&options, B4800);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        ret = cfsetospeed(&options, B4800);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
#elif _WIN32
        dcb.BaudRate = CBR_4800;
#endif /* __unix__ or _WIN32 */
        break;
    case UART_BAUD_9600:
#ifdef __unix__
        ret = cfsetispeed(&options, B9600);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        ret = cfsetospeed(&options, B9600);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
#elif _WIN32
        dcb.BaudRate = CBR_9600;
#endif /* __unix__ or _WIN32 */
        break;
    case UART_BAUD_19200:
#ifdef __unix__
        ret = cfsetispeed(&options, B19200);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        ret = cfsetospeed(&options, B19200);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
#elif _WIN32
        dcb.BaudRate = CBR_19200;
#endif /* __unix__ or _WIN32 */
        break;
    case UART_BAUD_38400:
#ifdef __unix__
        ret = cfsetispeed(&options, B38400);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        ret = cfsetospeed(&options, B38400);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
#elif _WIN32
        dcb.BaudRate = CBR_38400;
#endif /* __unix__ or _WIN32 */
        break;
    case UART_BAUD_57600:
#ifdef __unix__
        ret = cfsetispeed(&options, B57600);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        ret = cfsetospeed(&options, B57600);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
#elif _WIN32
        dcb.BaudRate = CBR_57600;
#endif /* __unix__ or _WIN32 */
        break;
    case UART_BAUD_115200:
#ifdef __unix__
        ret = cfsetispeed(&options, B115200);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        ret = cfsetospeed(&options, B115200);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
#elif _WIN32
        dcb.BaudRate = CBR_115200;
#endif /* __unix__ or _WIN32 */
#ifdef _WIN32
    case UART_BAUD_128000:
        dcb.BaudRate = CBR_128000;
        break;
    case UART_BAUD_256000:
        dcb.BaudRate = CBR_256000;
        break;
#elif __unix__
    case UART_BAUD_230400:
        ret = cfsetispeed(&options, B230400);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        ret = cfsetospeed(&options, B230400);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }

        break;
    case UART_BAUD_460800:
        ret = cfsetispeed(&options, B460800);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        ret = cfsetospeed(&options, B460800);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }

        break;
    case UART_BAUD_500000:
        ret = cfsetispeed(&options, B500000);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        ret = cfsetospeed(&options, B500000);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }

        break;
    case UART_BAUD_576000:
        ret = cfsetispeed(&options, B576000);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        ret = cfsetospeed(&options, B576000);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }

        break;
    case UART_BAUD_921600:
        ret = cfsetispeed(&options, B921600);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        ret = cfsetospeed(&options, B921600);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }

        break;
    case UART_BAUD_1000000:
        ret = cfsetispeed(&options, B1000000);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        ret = cfsetospeed(&options, B1000000);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }

        break;
    case UART_BAUD_1152000:
        ret = cfsetispeed(&options, B1152000);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        ret = cfsetospeed(&options, B1152000);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }

        break;
    case UART_BAUD_1500000:
        ret = cfsetispeed(&options, B1500000);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        ret = cfsetospeed(&options, B1500000);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }

        break;
    case UART_BAUD_2000000:
        ret = cfsetispeed(&options, B2000000);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        ret = cfsetospeed(&options, B2000000);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }

        break;
    case UART_BAUD_2500000:
        ret = cfsetispeed(&options, B2500000);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        ret = cfsetospeed(&options, B2500000);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }

        break;
    case UART_BAUD_3000000:
        ret = cfsetispeed(&options, B3000000);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        ret = cfsetospeed(&options, B3000000);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }

        break;
    case UART_BAUD_3500000:
        ret = cfsetispeed(&options, B3500000);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        ret = cfsetospeed(&options, B3500000);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }

        break;
    case UART_BAUD_4000000:
        ret = cfsetispeed(&options, B4000000);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }
        
        ret = cfsetospeed(&options, B4000000);
        
        if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
        }

        break;
#endif /* __unix__ */
        break;
    default:
        break;
    }
    
#ifdef __unix__
    ret = tcsetattr(uart->fd, TCSANOW, &options);
    
    if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
    }
#elif _WIN32
    ret = SetCommState(uart->h, &dcb);
    
    if (!ret) {
        dwerror = GetLastError();
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                  FORMAT_MESSAGE_IGNORE_INSERTS, 
                  NULL, 
                  dwerror, 
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
                  (LPTSTR) &lpmessage, 
                  0, 
                  NULL);
        printerr_uart_comm((const char *) lpmessage);
        LocalFree(lpmessage);
        return -1;
    }
#endif /* __unix__ or _WIN32 */
    
    return 0;
}

static int init_databits(uart_t *uart)
{
    int ret;
#ifdef __unix__
    struct termios options;
#elif _WIN32
    DCB dcb = { 0 };
    dcb.DCBlength = sizeof(dcb);
    DWORD dwerror;
    LPVOID lpmessage;
#endif /* __unix__ or _WIN32 */
    
#ifdef __unix__
    ret = tcgetattr(uart->fd, &options);
    
    if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
    }
#elif _WIN32
    ret = GetCommState(uart->h, &dcb);
    
    if (!ret) {
        dwerror = GetLastError();
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                  FORMAT_MESSAGE_IGNORE_INSERTS, 
                  NULL, 
                  dwerror, 
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
                  (LPTSTR) &lpmessage, 
                  0, 
                  NULL);
        printerr_uart_comm((const char *) lpmessage);
        LocalFree(lpmessage);
        return -1;
    }
#endif /* __unix__ or _WIN32 */

    switch (uart->data_bits) {
    case 5:
#ifdef __unix__
        options.c_cflag &= ~CSIZE;
        options.c_cflag |= CS5;
#elif _WIN32
        dcb.ByteSize = 5;
#endif /* __unix__ or _WIN32 */
        break;
    case 6:
#ifdef __unix__
        options.c_cflag &= ~CSIZE;
        options.c_cflag |= CS6;
#elif _WIN32
        dcb.ByteSize = 6;
#endif /* __unix__ or _WIN32 */
        break;
    case 7:
#ifdef __unix__
        options.c_cflag &= ~CSIZE;
        options.c_cflag |= CS7;
#elif _WIN32
        dcb.ByteSize = 7;
#endif /* __unix__ or _WIN32 */
        break;
    case 8:
#ifdef __unix__
        options.c_cflag &= ~CSIZE;
        options.c_cflag |= CS8;
#elif _WIN32
        dcb.ByteSize = 8;
#endif /* __unix__ or _WIN32 */
        break;
    default:
        break;
    }
    
#ifdef __unix__
    ret = tcsetattr(uart->fd, TCSANOW, &options);
    
    if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
    }
#elif _WIN32
    ret = SetCommState(uart->h, &dcb);
    
    if (!ret) {
        dwerror = GetLastError();
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                  FORMAT_MESSAGE_IGNORE_INSERTS, 
                  NULL, 
                  dwerror, 
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
                  (LPTSTR) &lpmessage, 
                  0, 
                  NULL);
        printerr_uart_comm((const char *) lpmessage);
        LocalFree(lpmessage);
        return -1;
    }
#endif /* __unix__ or _WIN32 */
    return 0;
}

static int init_parity(uart_t *uart)
{
    int ret;
#ifdef __unix__
    struct termios options;
#elif _WIN32
    DCB dcb = { 0 };
    dcb.DCBlength = sizeof(dcb);
    DWORD dwerror;
    LPVOID lpmessage;
#endif /* __unix__ or _WIN32 */
    
#ifdef __unix__
    ret = tcgetattr(uart->fd, &options);
    
    if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
    }
#elif _WIN32
    ret = GetCommState(uart->h, &dcb);
    
    if (!ret) {
        dwerror = GetLastError();
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                  FORMAT_MESSAGE_IGNORE_INSERTS, 
                  NULL, 
                  dwerror, 
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
                  (LPTSTR) &lpmessage, 
                  0, 
                  NULL);
        printerr_uart_comm((const char *) lpmessage);
        LocalFree(lpmessage);
        return -1;
    }
#endif /* __unix__ or _WIN32 */
    
    switch (uart->parity) {
    case UART_PARITY_NO:
#ifdef __unix__
        options.c_cflag &= ~PARENB;
#elif _WIN32
        dcb.Parity = NOPARITY;
#endif /* __unix__ or _WIN32 */
        break;
    case UART_PARITY_ODD:
#ifdef __unix__
        options.c_cflag |= PARENB;
        options.c_cflag |= PARODD;
#elif _WIN32
        dcb.Parity = ODDPARITY;
#endif /* __unix__ or _WIN32 */
        break;
    case UART_PARITY_EVEN:
#ifdef __unix__
        options.c_cflag |= PARENB;
        options.c_cflag &= ~PARODD;
#elif _WIN32
        dcb.Parity = EVENPARITY;
#endif /* __unix__ or _WIN32 */
        break;
    default:
        break;
    }
    
#ifdef __unix__
    ret = tcsetattr(uart->fd, TCSANOW, &options);
    
    if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
    }
#elif _WIN32
    ret = SetCommState(uart->h, &dcb);
    
    if (!ret) {
        dwerror = GetLastError();
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                  FORMAT_MESSAGE_IGNORE_INSERTS, 
                  NULL, 
                  dwerror, 
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
                  (LPTSTR) &lpmessage, 
                  0, 
                  NULL);
        printerr_uart_comm((const char *) lpmessage);
        LocalFree(lpmessage);
        return -1;
    }
#endif /* __unix__ or _WIN32 */
    
    return 0;
}

static int init_stopbits(uart_t *uart)
{
    int ret;
#ifdef __unix__
    struct termios options;
#elif _WIN32
    DCB dcb = { 0 };
    dcb.DCBlength = sizeof(dcb);
    DWORD dwerror;
    LPVOID lpmessage;
#endif /* __unix__ or _WIN32 */
    
#ifdef __unix__
    ret = tcgetattr(uart->fd, &options);
    
    if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
    }
#elif _WIN32
    ret = GetCommState(uart->h, &dcb);
    
    if (!ret) {
        dwerror = GetLastError();
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                  FORMAT_MESSAGE_IGNORE_INSERTS, 
                  NULL, 
                  dwerror, 
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
                  (LPTSTR) &lpmessage, 
                  0, 
                  NULL);
        printerr_uart_comm((const char *) lpmessage);
        LocalFree(lpmessage);
        return -1;
    }
#endif /* __unix__ or _WIN32 */
    
    switch (uart->stop_bits) {
    case 1:
#ifdef __unix__
        options.c_cflag &= ~CSTOPB;
#elif _WIN32
        dcb.StopBits = ONESTOPBIT;
#endif /* __unix__ or _WIN32 */
        break;
    case 2:
#ifdef __unix__
        options.c_cflag |= CSTOPB;
#elif _WIN32
        dcb.StopBits = TWOSTOPBITS;
#endif /* __unix__ or _WIN32 */
        break;
    default:
        break;
    }
    
#ifdef __unix__
    ret = tcsetattr(uart->fd, TCSANOW, &options);
    
    if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
    }
#elif _WIN32
    ret = SetCommState(uart->h, &dcb);
    
    if (!ret) {
        dwerror = GetLastError();
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                  FORMAT_MESSAGE_IGNORE_INSERTS, 
                  NULL, 
                  dwerror, 
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
                  (LPTSTR) &lpmessage, 
                  0, 
                  NULL);
        printerr_uart_comm((const char *) lpmessage);
        LocalFree(lpmessage);
        return -1;
    }
#endif /* __unix__ or _WIN32 */
    return 0;
}

static int init_flow(uart_t *uart)
{
    int ret;
#ifdef __unix__
    struct termios options;
#elif _WIN32
    DCB dcb = { 0 };
    dcb.DCBlength = sizeof(dcb);
    DWORD dwerror;
    LPVOID lpmessage;
#endif /* __unix__ or _WIN32 */
    
#ifdef __unix__
    ret = tcgetattr(uart->fd, &options);
    
    if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
    }
#elif _WIN32
    ret =  GetCommState(uart->h, &dcb);
    
    if (!ret) {
        dwerror = GetLastError();
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                  FORMAT_MESSAGE_IGNORE_INSERTS, 
                  NULL, 
                  dwerror, 
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
                  (LPTSTR) &lpmessage, 
                  0, 
                  NULL);
        printerr_uart_comm((const char *) lpmessage);
        LocalFree(lpmessage);
        return -1;
    }
#endif /* __unix__ or _WIN32 */
    
    switch (uart->flow_ctrl) {
    case UART_FLOW_NO:
#ifdef __unix__
        options.c_cflag &= ~CRTSCTS;
        options.c_iflag &= ~(IXON | IXOFF | IXANY);
#elif _WIN32
        dcb.fOutxCtsFlow = FALSE,
        dcb.fOutxDsrFlow = FALSE,
        dcb.fDtrControl = DTR_CONTROL_DISABLE;
        dcb.fRtsControl = RTS_CONTROL_DISABLE;
        dcb.fOutX = FALSE;
        dcb.fInX = FALSE;
#endif /* __unix__ or _WIN32 */
        break;
    case UART_FLOW_SOFTWARE:
#ifdef __unix__
        options.c_cflag &= ~CRTSCTS;
        options.c_iflag |= (IXON | IXOFF | IXANY);
#elif _WIN32
        dcb.fOutxCtsFlow = FALSE,
        dcb.fOutxDsrFlow = FALSE,
        dcb.fDtrControl = DTR_CONTROL_DISABLE;
        dcb.fRtsControl = RTS_CONTROL_DISABLE;
        dcb.fOutX = TRUE;
        dcb.fInX = TRUE;
#endif /* __unix__ or _WIN32 */
        break;
    case UART_FLOW_HARDWARE:
#ifdef __unix__
        options.c_cflag |= CRTSCTS;
        options.c_iflag &= ~(IXON | IXOFF | IXANY);
#elif _WIN32
        dcb.fOutxCtsFlow = TRUE,
        dcb.fOutxDsrFlow = TRUE,
        dcb.fDtrControl = DTR_CONTROL_ENABLE;
        dcb.fRtsControl = RTS_CONTROL_ENABLE;
        dcb.fOutX = FALSE;
        dcb.fInX = FALSE;
#endif /* __unix__ or _WIN32 */
        break;
    default:
        break;
    }
    
#ifdef __unix__
    ret = tcsetattr(uart->fd, TCSANOW, &options);
    
    if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
    }
#elif _WIN32
    ret =  SetCommState(uart->h, &dcb);
    
    if (!ret) {
        dwerror = GetLastError();
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                  FORMAT_MESSAGE_IGNORE_INSERTS, 
                  NULL, 
                  dwerror, 
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
                  (LPTSTR) &lpmessage, 
                  0, 
                  NULL);
        printerr_uart_comm((const char *) lpmessage);
        LocalFree(lpmessage);
        return -1;
    }
#endif /* __unix__ or _WIN32 */
    
    return 0;
}

static int init(uart_t *uart)
{
    int ret;
#ifdef __unix__
    struct termios options;
#endif /* __unix__ */
    
    if (!uart) {
        printerr_uart_type_invalid();
        return -1;
    }
    
#ifdef __unix__
    /* set non-blocking mode*/
    ret = fcntl(uart->fd, F_SETFL, FNDELAY);
    
    if (ret == -1) {
        printerr_fcntl(strerror(errno));
        return -1;
    }
#endif /* __unix__ */
    
    /* set baud rate */
    ret = init_baud(uart);
    
    if (ret == -1)
        return -1;
    
    /* set data bits */
    ret = init_databits(uart);
    
    if (ret == -1)
        return -1;
    
    /* set parity */
    ret = init_parity(uart);
    
    if (ret == -1)
        return -1;
    
    /* set stop bits */
    ret = init_stopbits(uart);
    
    if (ret == -1)
        return -1;
    
    /* set flow control */
    ret = init_flow(uart);
    
    if (ret == -1)
        return -1;
    
#ifdef __unix__
    ret = tcgetattr(uart->fd, &options);
    
    if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
    }
    
    /* set raw input mode */
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    
    /* enable receiver and set local mode */
    options.c_cflag |= (CLOCAL | CREAD);
    ret = tcsetattr(uart->fd, TCSANOW, &options);
    
    if (ret == -1) {
            printerr_uart_termios(strerror(errno));
            return -1;
    }
#endif /* __unix__ */
    return 0;
}

uart_t *uart_open(const char *dev, int baud, const char *opt)
{
    int ret;
#ifdef __unix__
    int fd;
#elif _WIN32
    HANDLE h;
    DWORD dwerror;
    LPVOID lpmessage;
#endif /* __unix__ or _WIN32 */
    uart_t *p;
    
    p = malloc(sizeof(uart_t));
    
    if (!p) {
        printerr_alloc(strerror(errno));
        return NULL;
    }
    
    if (strlen(dev) >= BUFSZ) {
        printerr_uart_dev_too_long();
        return NULL;
    }
    
    strcpy(p->dev, dev);
#ifdef __unix__
    fd = open(p->dev, O_RDWR | O_NOCTTY | O_NDELAY);
    
    if (fd == -1) {
        printerr_open(strerror(errno));
        return NULL;
    }
    
    p->fd = fd;
#elif _WIN32
    h = CreateFile((LPCTSTR) dev,
                   GENERIC_READ | GENERIC_WRITE,
                   0,
                   NULL,
                   OPEN_EXISTING,
                   0,
                   NULL);
    
    if (h == INVALID_HANDLE_VALUE) {
        dwerror = GetLastError();
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                  FORMAT_MESSAGE_IGNORE_INSERTS, 
                  NULL, 
                  dwerror, 
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
                  (LPTSTR) &lpmessage, 
                  0, 
                  NULL);
        printerr_open((const char *) lpmessage);
        LocalFree(lpmessage);
        return NULL;
    }
    
    p->h = h;
#endif /* __unix__ or _WIN32 */
    ret = parse_option(p, opt);
    
    if (ret == -1)
        return NULL;
    
    if (!is_baud_valid(baud)) {
        printerr_uart_baud_invalid();
        return NULL;
    }
    
    p->baud = baud;
    ret = init(p);
    
    if (ret == -1)
        return NULL;
    
    return p;
}

void uart_close(uart_t *uart)
{
    if (!uart) {
        printerr_uart_type_invalid();
        return;
    }
    
#ifdef __unix__
    close(uart->fd);
#elif _WIN32
    CloseHandle(uart->h);
#endif /* __unix__ or _WIN32 */
    free(uart);
    uart = NULL;
}

int uart_send(uart_t *uart, char *send_buf, int len)
{
    int ret;
#ifdef _WIN32
    DWORD dwbytestowrite = (DWORD) len;
    DWORD dwbyteswritten;
    DWORD dwerror;
    LPVOID lpmessage;
#endif /* _WIN32 */
    
    if (!uart) {
        printerr_uart_type_invalid();
        return 0;
    }
    
#ifdef __unix__
    ret = write(uart->fd, send_buf, len);
    
    if (ret == -1) {
        printerr_write(strerror(errno));
        return -1;
    }
#elif _WIN32
    ret = WriteFile(uart->h, 
                    (LPVOID) send_buf, 
                    dwbytestowrite, 
                    &dwbyteswritten, 
                    NULL);
    
    if (!ret) {
        dwerror = GetLastError();
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                  FORMAT_MESSAGE_IGNORE_INSERTS, 
                  NULL, 
                  dwerror, 
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
                  (LPTSTR) &lpmessage, 
                  0, 
                  NULL);
        printerr_write((const char *) lpmessage);
        LocalFree(lpmessage);
        return -1;
    }
    
    ret = (int) dwbyteswritten;
#endif /* __unix__ or _WIN32 */
    
    if (ret != len) {
        printerr_write("couldn't send all bytes");
        return ret;
    }
    
    return ret;
}

int uart_recv(uart_t *uart, char *recv_buf, int len)
{
    int ret = 0;
#ifdef _WIN32
    DWORD dwbytesread = (DWORD) len;
    DWORD dwerror;
    LPVOID lpmessage;
#endif /* _WIN32 */
    
    if (!uart) {
        printerr_uart_type_invalid();
        return 0;
    }
    
#ifdef __unix__
    ret = read(uart->fd, recv_buf, len);
    
    if (ret == -1) {
        printerr_read(strerror(errno));
        return -1;
    }
#elif _WIN32
    ret = ReadFile(uart->h, 
                   (LPVOID) recv_buf, 
                   (DWORD) len, 
                   &dwbytesread, 
                   NULL);
    
    if (!ret) {
        dwerror = GetLastError();
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                  FORMAT_MESSAGE_IGNORE_INSERTS, 
                  NULL, 
                  dwerror, 
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
                  (LPTSTR) &lpmessage, 
                  0, 
                  NULL);
        printerr_read((const char *) lpmessage);
        LocalFree(lpmessage);
        return -1;
    }
    
    ret = (int) dwbytesread;
#endif /* __unix__ or _WIN32 */
    return ret;
}

void uart_pin_set(uart_t *uart, int pin, int state)
{
    int ret;
#ifdef __unix__
    int status;
#elif _WIN32
    DWORD dwfunc;
    DWORD dwerror;
    LPVOID lpmessage;
#endif /* __unix__ or _WIN32 */
    
    if (!uart) {
        printerr_uart_type_invalid();
        return;
    }
    
#ifdef __unix__
    ret = ioctl(uart->fd, TIOCMGET, &status);
    
    if (ret == -1) {
        printerr_ioctl(strerror(errno));
        return;
    }
#endif /* __unix__ */
    
    switch (pin) {
    case UART_PIN_RTS:
        if (state == UART_PIN_HIGH)
#ifdef __unix__
            status |= TIOCM_RTS;
        else
            status &= ~TIOCM_RTS;
#elif _WIN32
            dwfunc = SETRTS;
        else
            dwfunc = CLRRTS;
#endif /* __unix__ or _WIN32 */
        break;
    case UART_PIN_DTR:
        if (state == UART_PIN_HIGH)
#ifdef __unix__
            status |= TIOCM_DTR;
        else
            status &= ~TIOCM_DTR;
#elif _WIN32
            dwfunc = SETDTR;
        else
            dwfunc = CLRDTR;
#endif /* __unix__ or _WIN32 */
        break;
    default:
        printerr_uart_pin_invalid();
    }
    
#ifdef __unix__
    ioctl(uart->fd, TIOCMSET, status);
    
    if (ret == -1) {
        printerr_ioctl(strerror(errno));
        return;
    }
#elif _WIN32
    ret = EscapeCommFunction(uart->h, dwfunc);
    
    if (!ret) {
        dwerror = GetLastError(); 
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                  FORMAT_MESSAGE_IGNORE_INSERTS, 
                  NULL, 
                  dwerror, 
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
                  (LPTSTR) &lpmessage, 
                  0, 
                  NULL);
        printerr_uart_comm((const char *) lpmessage);
        LocalFree(lpmessage);
        return;
    }
#endif /* __unix__ or _WIN32 */
}

int uart_pin_get(uart_t *uart, int pin)
{
    int ret = 0;
#ifdef __unix__
    int status;
#elif _WIN32
    DWORD dwmstat;
    DWORD dwerror;
    LPVOID lpmessage;
#endif /* __unix__ or _WIN32 */
    
    if (!uart) {
        printerr_uart_type_invalid();
        return 0;
    }
    
#ifdef __unix__
    ioctl(uart->fd, TIOCMGET, &status);
    
    if (ret == -1) {
        printerr_ioctl(strerror(errno));
        return -1;
    }
#elif _WIN32
    ret = GetCommModemStatus(uart->h, &dwmstat);

    if (!ret) {
        dwerror = GetLastError();
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                  FORMAT_MESSAGE_IGNORE_INSERTS, 
                  NULL, 
                  dwerror, 
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
                  (LPTSTR) &lpmessage, 
                  0, 
                  NULL);
        printerr_uart_comm((const char *) lpmessage);
        LocalFree(lpmessage);
        return -1;
    }
#endif /* __unix__ or _WIN32 */
    
    switch (pin) {
    case UART_PIN_CTS:
#ifdef __unix__
        if (status & TIOCM_CTS)
#elif _WIN32
        if (dwmstat == MS_CTS_ON)
#endif /* __unix__ or _WIN32 */
            ret = UART_PIN_HIGH;
        else
            ret = UART_PIN_LOW;
        break;
    case UART_PIN_DSR:
#ifdef __unix__
        if (status & TIOCM_DSR)
#elif _WIN32
        if (dwmstat == MS_DSR_ON)
#endif /* __unix__ or _WIN32 */
            ret = UART_PIN_HIGH;
        else
            ret = UART_PIN_LOW;
        break;
    case UART_PIN_DCD:
#ifdef __unix__
        if (status & TIOCM_CAR)
#elif _WIN32
        if (dwmstat == MS_RLSD_ON)
#endif /* __unix__ or _WIN32 */
            ret = UART_PIN_HIGH;
        else
            ret = UART_PIN_LOW;
        break;
    case UART_PIN_RI:
#ifdef __unix__
        if (status & TIOCM_RI)
#elif _WIN32
        if (dwmstat == MS_RING_ON)
#endif /* __unix__ or _WIN32 */
            ret = UART_PIN_HIGH;
        else
            ret = UART_PIN_LOW;
        break;
    default:
        printerr_uart_pin_invalid();
        ret = -1;
    }
    
    return ret;
}

int uart_bytes_get(uart_t *uart)
{
    int ret = 0;
#ifdef _WIN32
    COMSTAT comst;
    DWORD dwerror;
    LPVOID lpmessage;
#endif /* _WIN32 */
    
    if (!uart) {
        printerr_uart_type_invalid();
        return 0;
    }
    
#ifdef __unix__
    ioctl(uart->fd, FIONREAD, &ret);
    
    if (ret == -1) {
        printerr_ioctl(strerror(errno));
        return -1;
    }
#elif _WIN32
    ret = ClearCommError(uart->h, &dwerror, &comst);
    
    if (!ret) {
        dwerror = GetLastError();
        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                  FORMAT_MESSAGE_IGNORE_INSERTS, 
                  NULL, 
                  dwerror, 
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
                  (LPTSTR) &lpmessage, 
                  0, 
                  NULL);
        printerr_uart_comm((const char *) lpmessage);
        LocalFree(lpmessage);
        return -1;
    }
    
    ret = (int) comst.cbInQue;
#endif /* __unix__ or _WIN32 */
    
    return ret;
}

void uart_baud_set(uart_t *uart, int baud)
{
    if (!uart) {
        printerr_uart_type_invalid();
        return;
    }
    
    if (!is_baud_valid(baud)) {
        printerr_uart_baud_invalid();
        return;
    }
    
    uart->baud = baud;
    init_baud(uart);
}

int uart_baud_get(uart_t *uart)
{
    if (!uart) {
        printerr_uart_type_invalid();
        return 0;
    }
    
    return uart->baud; 
}

#ifdef __unix__
int uart_fd_get(uart_t *uart)
{
    if (!uart) {
        printerr_uart_type_invalid();
        return -1;
    }
    
    return uart->fd;
}
#elif _WIN32
HANDLE uart_handle_get(uart_t *uart)
{
    if (!uart) {
        printerr_uart_type_invalid();
        return INVALID_HANDLE_VALUE;
    }
    
    return uart->h;
}
#endif /* __unix__ or _WIN32 */

char *uart_dev_get(uart_t *uart)
{
    char *p;
    
    if (!uart) {
        printerr_uart_type_invalid();
        return NULL;
    }
    
    p = malloc(strlen(uart->dev) + 1);
    
    if (!p) {
        printerr_alloc(strerror(errno));
        return NULL;
    }
    
    strcpy(p, uart->dev);
    return p; 
}

void uart_databits_set(uart_t *uart, int data_bits)
{
    if (!uart) {
        printerr_uart_type_invalid();
        return;
    }
    
    if (!is_databits_valid(data_bits))
    {
        printerr_uart_data_invalid();
        return;
    }
    
    uart->data_bits = data_bits;
    init_databits(uart);
}

int uart_databits_get(uart_t *uart)
{
    if (!uart) {
        printerr_uart_type_invalid();
        return 0;
    }
    
    return uart->data_bits;
}

void uart_parity_set(uart_t *uart, int parity)
{
    if (!uart) {
        printerr_uart_type_invalid();
        return;
    }
    
    if (!is_parity_valid(parity))
    {
        printerr_uart_parity_invalid();
        return;
    }
    
    uart->parity = parity;
    init_parity(uart);
}

int uart_parity_get(uart_t *uart)
{
    if (!uart) {
        printerr_uart_type_invalid();
        return 0;
    }
    
    return uart->parity;
}

void uart_stopbits_set(uart_t *uart, int stop_bits)
{
    if (!uart) {
        printerr_uart_type_invalid();
        return;
    }
    
    if (!is_stopbits_valid(stop_bits))
    {
        printerr_uart_stop_invalid();
        return;
    }
    
    uart->stop_bits = stop_bits;
    init_stopbits(uart);
}

int uart_stopbits_get(uart_t *uart)
{
    if (!uart) {
        printerr_uart_type_invalid();
        return 0;
    }
    
    return uart->stop_bits;
}

void uart_flow_set(uart_t *uart, int flow_ctrl)
{
    if (!uart) {
        printerr_uart_type_invalid();
        return;
    }
    
    if (!is_flow_valid(flow_ctrl))
    {
        printerr_uart_flow_invalid();
        return;
    }
    
    uart->flow_ctrl = flow_ctrl;
    init_flow(uart);
}

int uart_flow_get(uart_t *uart)
{
    if (!uart) {
        printerr_uart_type_invalid();
        return 0;
    }
    
    return uart->flow_ctrl;
}
