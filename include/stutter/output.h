/*
 * Header Name:	output.h
 * Description:	General Output Macros Header
 */

#ifndef _STUTTER_OUTPUT_H
#define _STUTTER_OUTPUT_H

#include <stdio.h>
#include <stdarg.h>

#include CONFIG_H
#include <stutter/signal.h>

static inline int OUTPUT_ERROR(const char *fmt, ...) {
	va_list va;
	char buffer[LARGE_STRING_SIZE];

	va_start(va, fmt);
	if (vsnprintf(buffer, LARGE_STRING_SIZE, fmt, va) >= 0)
		signal_named_emit(signal_table, "output.error", buffer);
	va_end(va);
	return(0);
}

static inline int OUTPUT_STATUS(const char *fmt, ...) {
	va_list va;
	char buffer[LARGE_STRING_SIZE];

	va_start(va, fmt);
	if (vsnprintf(buffer, LARGE_STRING_SIZE, fmt, va) >= 0)
		signal_named_emit(signal_table, "output.status", buffer);
	va_end(va);
	return(0);
}

#endif


