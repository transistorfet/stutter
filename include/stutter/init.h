/*
 * Header Name:		init.h
 * Version:		0.1
 * Description:		Initialization Structures and Macros
 */

#ifndef _STUTTER_INIT_H
#define _STUTTER_INIT_H

#include <stutter/type.h>
#include <stutter/signal.h>
#include <stutter/variable.h>

/*** Signals ***/

struct handler_prototype_s {
	char *name;
	void *index;
	int priority;
	signal_t func;
	void *ptr;
};

#define ADD_HANDLER(name, index, priority, func, env)	\
	{ name, index, priority, (signal_t) func, env },

/*** Commands ***/

struct command_prototype_s {
	char *name;
	callback_t func;
	void *ptr;
};

#define ADD_COMMAND(name, func)	\
	{ name, (callback_t) func, NULL },

#define ADD_COMMAND_ENV(name, func, env)	\
	{ name, (callback_t) func, env },

/*** Keys ***/

#define BIND_KEY(key, var, params)	\
	bind_key(NULL, key, find_variable(NULL, var), create_string(params));

#endif


