/**
 *
 * File Name: error_uart.c
 * Title    : UART library error messages source
 * Project  : libuart
 * Author   : Copyright (C) 2018 Johannes Krottmayer <krjdev@gmail.com>
 * Created  : 2018-05-21
 * Modified : 2018-06-19
 * Revised  : 
 * Version  : 0.2.0.0
 * License  : ISC (see file LICENSE.txt)
 *
 * NOTE: This code is currently below version 1.0, and therefore is considered
 * to be lacking in some functionality or documentation, or may not be fully
 * tested. Nonetheless, you can expect most functions to work.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "error.h"

void printerr_uart_type_invalid(void)
{
    printerr();
#ifdef __unix__
    fprintf(stderr, "invalid uart_type (uart_t)\n");
#elif _WIN32
    fprintf(stderr, "invalid uart_type (uart_t)\r\n");
#endif /* __unix__ or _WIN32 */
}

void printerr_uart_dev_too_long(void)
{
    printerr();
#ifdef __unix__
    fprintf(stderr, "device/file name too long\n");
#elif _WIN32
    fprintf(stderr, "device/file name too long\r\n");
#endif /* __unix__ or _WIN32 */
}

void printerr_uart_opt_invalid(const char *err_msg)
{
    printerr();
#ifdef __unix__
    if (err_msg == NULL)
        fprintf(stderr, "invalid option\n");
    else
        fprintf(stderr, "invalid option (%s)\n", err_msg);
#elif _WIN32
    if (err_msg == NULL)
        fprintf(stderr, "invalid option\r\n");
    else
        fprintf(stderr, "invalid option (%s)\r\n", err_msg);
#endif /* __unix__ or _WIN32 */
}

void printerr_uart_baud_invalid(void)
{
    printerr();
#ifdef __unix__
    fprintf(stderr, "invalid baud rate\n");
#elif _WIN32
    fprintf(stderr, "invalid baud rate\r\n");
#endif /* __unix__ or _WIN32 */
}

void printerr_uart_data_invalid(void)
{
    printerr();
#ifdef __unix__
    fprintf(stderr, "invalid data bits\n");
#elif _WIN32
    fprintf(stderr, "invalid data bits\r\n");
#endif /* __unix__ or _WIN32 */
}

void printerr_uart_parity_invalid(void)
{
    printerr();
#ifdef __unix__
    fprintf(stderr, "invalid parity\n");
#elif _WIN32
    fprintf(stderr, "invalid parity\r\n");
#endif /* __unix__ or _WIN32 */
}

void printerr_uart_stop_invalid(void)
{
    printerr();
#ifdef __unix__
    fprintf(stderr, "invalid stop bits\n");
#elif _WIN32
    fprintf(stderr, "invalid stop bits\r\n");
#endif /* __unix__ or _WIN32 */
}

void printerr_uart_flow_invalid(void)
{
    printerr();
#ifdef __unix__
    fprintf(stderr, "invalid flow control\n");
#elif _WIN32
    fprintf(stderr, "invalid flow control\r\n");
#endif /* __unix__ or _WIN32 */
}

void printerr_uart_pin_invalid(void)
{
    printerr();
#ifdef __unix__
    fprintf(stderr, "invalid control pin\n");
#elif _WIN32
    fprintf(stderr, "invalid control pin\r\n");
#endif /* __unix__ or _WIN32 */
}

#ifdef __unix__
void printerr_uart_termios(const char *err_msg)
{
    printerr();
    fprintf(stderr, "termios failed (%s)\n", err_msg);
}
#elif _WIN32
void printerr_uart_comm(const char *err_msg)
{
    printerr();
    fprintf(stderr, "comm function failed (%s)\r\n", err_msg);
}
#endif /* __unix__ or _WIN32 */
