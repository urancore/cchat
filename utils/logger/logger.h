#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>

#define CLR_RESET   "\033[0m"
#define CLR_GRAY    "\033[90m"
#define CLR_RED     "\033[31m"
#define CLR_GREEN   "\033[32m"
#define CLR_YELLOW  "\033[33m"
#define CLR_BLUE    "\033[34m"
#define CLR_CYAN    "\033[36m"

#define DEF_LOG_PREFIX "root"

typedef enum {
	FATAL   = 0,
	ERROR   = 1,
	WARN    = 2,
	INFO    = 3,
	DEBUG   = 4,
	TRACE   = 5
} LogLevel;


typedef struct {
	LogLevel log_level;
	char prefix[32];
	FILE *stream;

	unsigned char show_timestamp;
	unsigned char use_color;
} Logger;

Logger logger_init(LogLevel level, char *prefix, FILE *stream, unsigned char show_timestamp, unsigned char use_color);
void print_log(Logger *l, LogLevel level, const char *fmt, ...);

void log_fatal(Logger *l, const char *fmt, ...);
void log_error(Logger *l, const char *fmt, ...);
void log_warn(Logger *l, const char *fmt, ...);
void log_info(Logger *l, const char *fmt, ...);
void log_debug(Logger *l, const char *fmt, ...);
void log_trace(Logger *l, const char *fmt, ...);

#endif
