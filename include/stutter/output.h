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
	// (snprintf(buffer, STRING_SIZE, fmt, va) >= 0)
	return(0);
}

static inline int OUTPUT_STATUS(const char *fmt, ...) {
	// (snprintf(buffer, STRING_SIZE, fmt, va) >= 0)
	return(0);
}

#endif


