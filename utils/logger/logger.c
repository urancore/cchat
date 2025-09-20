#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#include "logger.h"

#define MAX_LOG_BUF_SIZE 1024

static const char *log_level_str[] = {"FATAL", "ERROR", "WARN", "INFO", "DEBUG", "TRACE"};

static const char *log_level_colors[] = {CLR_RED, CLR_RED, CLR_YELLOW, CLR_GREEN, CLR_CYAN, CLR_GRAY};

Logger logger_init(LogLevel level, char *prefix, FILE *stream, unsigned char show_timestamp, unsigned char use_color)
{
	Logger l;
	l.log_level = level;

	if (prefix)
		strncpy(l.prefix, prefix, sizeof(l.prefix) - 1);
	else
		l.prefix[0] = '\0';

	l.stream = (stream != NULL) ? stream : stdout;

	l.show_timestamp = show_timestamp;
	l.use_color = use_color;
	return l;
}

static void _vprint_log(Logger *l, LogLevel level, const char *fmt, va_list args)
{
	if (l->log_level < level) return;

	char LogMessage[MAX_LOG_BUF_SIZE];
	char time_buf[24] = "";

	if (l->show_timestamp) {
		time_t timer = time(NULL);

	#ifdef _WIN32
		struct tm tm_info;
		localtime_s(&tm_info, &timer);
		strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", &tm_info);
	#else
		struct tm *tm_ptr = localtime(&timer);
		if (tm_ptr) {
		strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", tm_ptr);
		}
	#endif
	}

	if (l->use_color) {
		snprintf(LogMessage, sizeof(LogMessage), "%s[%s]%s %s%-5s%s %s%s%s ",
			CLR_GRAY, time_buf, CLR_RESET,
			log_level_colors[level], log_level_str[level], CLR_RESET,
			CLR_CYAN, (l->prefix[0] ? l->prefix : DEF_LOG_PREFIX), CLR_RESET);
	} else {
		snprintf(LogMessage, sizeof(LogMessage), "[%s] %-5s %s ",
			time_buf, log_level_str[level],
			(l->prefix[0] ? l->prefix : DEF_LOG_PREFIX));
	}

	vsnprintf(LogMessage + strlen(LogMessage),
		sizeof(LogMessage) - strlen(LogMessage), fmt, args);

	fprintf(l->stream, "%s\n", LogMessage);
}


void print_log(Logger *l, LogLevel level, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	_vprint_log(l, level, fmt, args);
	va_end(args);
}

void log_info(Logger *l, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	_vprint_log(l, LOG_INFO, fmt, args);
	va_end(args);
}

void log_fatal(Logger *l, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	_vprint_log(l, LOG_FATAL, fmt, args);
	va_end(args);
}

void log_error(Logger *l, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	_vprint_log(l, LOG_ERROR, fmt, args);
	va_end(args);
}

void log_warn(Logger *l, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	_vprint_log(l, LOG_WARN, fmt, args);
	va_end(args);
}

void log_debug(Logger *l, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	_vprint_log(l, LOG_DEBUG, fmt, args);
	va_end(args);
}

void log_trace(Logger *l, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	_vprint_log(l, LOG_TRACE, fmt, args);
	va_end(args);
}
