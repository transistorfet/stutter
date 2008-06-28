/*
 * Header Name:	output.h
 * Description:	General Output Macros Header
 */

#ifndef _STUTTER_OUTPUT_H
#define _STUTTER_OUTPUT_H

#include <stdio.h>
#include <stdarg.h>

#include <stutter/signal.h>

static inline int OUTPUT_ERROR(const char *fmt, ...) {
	char buffer[LARGE_STRING_SIZE];

	if (snprintf(buffer, LARGE_STRING_SIZE, fmt, va) >= 0)
		signal_named_emit(signal_table, "output.error", buffer);
	return(0);
}

static inline int OUTPUT_STATUS(const char *fmt, ...) {
	char buffer[LARGE_STRING_SIZE];

	if (snprintf(buffer, LARGE_STRING_SIZE, fmt, va) >= 0)
		signal_named_emit(signal_table, "output.status", buffer);
	return(0);
}

#endif


