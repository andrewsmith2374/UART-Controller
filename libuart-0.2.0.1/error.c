/**
 *
 * File Name: error.c
 * Title    : Generic error messages source
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

void printerr(void)
{
    fprintf(stderr, "[libuart] error: ");
}

void printerr_alloc(const char *err_msg)
{
    printerr();
#ifdef __unix__
    fprintf(stderr, "couldn't allocate memory (%s)\n", err_msg);
#elif _WIN32
    fprintf(stderr, "couldn't allocate memory (%s)\r\n", err_msg);
#endif /* __unix__ or _WIN32 */
}

void printerr_realloc(const char *err_msg)
{
    printerr();
#ifdef __unix__
    fprintf(stderr, "couldn't reallocate memory (%s)\n", err_msg);
#elif _WIN32
    fprintf(stderr, "couldn't reallocate memory (%s)\r\n", err_msg);
#endif /* __unix__ or _WIN32 */
}

void printerr_open(const char *err_msg)
{
    printerr();
#ifdef __unix__
    fprintf(stderr, "couldn't open (%s)\n", err_msg);
#elif _WIN32
    fprintf(stderr, "couldn't open (%s)\r\n", err_msg);
#endif /* __unix__ or _WIN32 */
}

void printerr_read(const char *err_msg)
{
    printerr();
#ifdef __unix__
    fprintf(stderr, "couldn't read (%s)\n", err_msg);
#elif _WIN32
    fprintf(stderr, "couldn't read (%s)\r\n", err_msg);
#endif /* __unix__ or _WIN32 */
}

void printerr_write(const char *err_msg)
{
    printerr();
#ifdef __unix__
    fprintf(stderr, "couldn't write (%s)\n", err_msg);
#elif _WIN32
    fprintf(stderr, "couldn't write (%s)\r\n", err_msg);
#endif /* __unix__ or _WIN32 */
}

#ifdef __unix__
void printerr_ioctl(const char *err_msg)
{
    printerr();
    fprintf(stderr, "ioctl failed (%s)\n", err_msg);
}

void printerr_fcntl(const char *err_msg)
{
    printerr();
    fprintf(stderr, "fcntl failed (%s)\n", err_msg);
}
#endif /* __unix__ */

void printerr_pointer_null(void)
{
    printerr();
#ifdef __unix__
    fprintf(stderr, "invalid pointer (NULL)\n");
#elif _WIN32
    fprintf(stderr, "invalid pointer (NULL)\r\n");
#endif /* __unix__ or _WIN32 */
}

void printerr_string_null(void)
{
    printerr();
#ifdef __unix__
    fprintf(stderr, "invalid string (NULL)\n");
#elif _WIN32
    fprintf(stderr, "invalid string (NULL)\r\n");
#endif /* __unix__ or _WIN32 */
}
