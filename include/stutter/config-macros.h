/*
 * Header Name:	config-macros.h
 * Description:	Configuration Macros Header
 */

#ifndef _CONFIG_MACROS_H
#define _CONFIG_MACROS_H

#include <stdio.h>
#include <stdarg.h>

#include <stutter/utils.h>
#include <stutter/signal.h>
#include <stutter/frontend.h>

#define DO_NOTHING() \
	;

#define EMIT_SIGNAL(obj, name, ptr) \
	emit_signal(obj, name, ptr);

#define EMIT_STRING(obj, name, ...) \
	util_emit_str(obj, name, __VA_ARGS__);

#define PRINT_TO_CURRENT(...) {						\
	void *window;							\
	char buffer[STRING_SIZE];					\
	if (((window = fe_get_target(NULL, "text")))			\
	    && (snprintf(buffer, STRING_SIZE, __VA_ARGS__) >= 0))	\
		fe_print(window, buffer);				\
}

#endif


