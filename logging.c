/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <syslog.h>
#include "logging.h"
#include "slog.h"

void user_log_init(void) {
//    openlog("controller_app", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
    slog_init("/var/log/usr/controller", NULL, 0, 1);
}

void user_write_log(const char *format, ...) {
    char buffer[4096];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof (buffer), format, args);
    va_end(args);

//    syslog(LOG_DEBUG, "%s", buffer);
    slog(0, 0, "%s", buffer);
}