/**
 *
 * File Name: error.h
 * Title    : UART library error messages header
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

#ifndef ERROR_H
#define ERROR_H

extern void printerr(void);
extern void printerr_alloc(const char *err_msg);
extern void printerr_realloc(const char *err_msg);
extern void printerr_pointer_null(void);
extern void printerr_string_null(void);
extern void printerr_open(const char *err_msg);
extern void printerr_read(const char *err_msg);
extern void printerr_write(const char *err_msg);
#ifdef __unix__
extern void printerr_ioctl(const char *err_msg);
extern void printerr_fcntl(const char *err_msg);
#endif /* __unix__ */

/* error_uart.c */
extern void printerr_uart_type_invalid(void);
extern void printerr_uart_dev_too_long(void);
extern void printerr_uart_opt_invalid(const char *err_msg);
extern void printerr_uart_baud_invalid(void);
extern void printerr_uart_data_invalid(void);
extern void printerr_uart_parity_invalid(void);
extern void printerr_uart_stop_invalid(void);
extern void printerr_uart_flow_invalid(void);
extern void printerr_uart_pin_invalid(void);
#ifdef __unix__
extern void printerr_uart_termios(const char *err_msg);
#elif _WIN32
extern void printerr_uart_comm(const char *err_msg);
#endif /* __unix__ or _WIN32 */

#endif
