/*
 * Header Name:	config.macros.h
 * Description:	Configuration Macros Header
 */

#ifndef _CONFIG_MACROS_H
#define _CONFIG_MACROS_H

#include <stdarg.h>

#include <stutter/utils.h>
#include <stutter/signal.h>
#include <stutter/frontend.h>

#define DO_NOTHING() \
	;

#define EMIT_SIGNAL(name, index, ptr) \
	emit_signal(name, index, ptr);

#define EMIT_STRING(name, index, ...) \
	util_emit_str(name, index, __VA_ARGS__);

#define PRINT_TO_CURRENT(...) {					\
	void *window;						\
	char buffer[STRING_SIZE];				\
	if (((window = fe_current_widget("text", NULL)) || (window = fe_first_widget("text", NULL)))	\
	    && (snprintf(buffer, STRING_SIZE, __VA_ARGS__) >= 0))	\
		fe_print(window, buffer);			\
}

#endif


