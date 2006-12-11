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

/*** Modules ***/

typedef int (*module_init_t)(void);
typedef int (*module_release_t)(void);

struct module_prototype_s {
	module_init_t init;
	module_release_t release;
};

#define STATIC_MODULE(name)	\
	{ (module_init_t) init_##name, (module_release_t) release_##name },

#define DEFINE_MODULE_LIST(name, list)			\
	static struct module_prototype_s name[] = {	\
		list					\
		{ NULL, NULL }				\
	}

#define INIT_MODULE_LIST(list) {			\
	int i;						\
	for (i = 0;list[i].init;i++)			\
		list[i].init();				\
}

#define RELEASE_MODULE_LIST(list) {			\
	int i;						\
	for (i = 0;list[i].release;i++)			\
		list[i].release();			\
}

/*** Types ***/

typedef int (*type_init_t)(void);

struct type_prototype_s {
	type_init_t func;
};

#define ADD_TYPE(func)	\
	{ (type_init_t) func },

#define DEFINE_TYPE_LIST(name, list)			\
	static struct type_prototype_s name[] = {	\
		list					\
		{ NULL }				\
	}

#define ADD_TYPE_LIST(list) {				\
	int i;						\
	for (i = 0;list[i].func;i++)			\
		list[i].func();				\
}

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

#define DEFINE_HANDLER_LIST(name, list)			\
	static struct handler_prototype_s name[] = {	\
		list					\
		{ NULL, NULL, 0, NULL, NULL }		\
	}

#define ADD_HANDLER_LIST(list) {			\
	int i;						\
	for (i = 0;list[i].name;i++)			\
		signal_connect(list[i].name, list[i].index, list[i].priority, (signal_t) list[i].func, list[i].ptr);	\
}

#define REMOVE_HANDLER_LIST(list) {			\
	int i;						\
	for (i = 0;list[i].name;i++)			\
		signal_disconnect(list[i].name, list[i].index, (signal_t) list[i].func, list[i].ptr);	\
}

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

#define DEFINE_COMMAND_LIST(name, list)			\
	static struct command_prototype_s name[] = {	\
		list					\
		{ NULL, NULL, NULL }			\
	}

#define ADD_COMMAND_LIST(table, list) {				\
	int i;							\
	struct type_s *type;					\
								\
	if ((type = find_type("command")) && type->create) {	\
		for (i = 0;list[i].name;i++)			\
			add_variable(table->value, type, list[i].name, 0, "%r%p", list[i].func, list[i].ptr);	\
	}							\
}

/*** Keys ***/

struct key_prototype_s {
	char *key;
	char *var;
	char *params;
};

#define BIND_KEY(key, var, params)	\
	{ key, var, params },

#define DEFINE_KEY_LIST(name, list)			\
	static struct key_prototype_s name[] = {	\
		list					\
		{ NULL, NULL, NULL }			\
	}

#define ADD_KEY_LIST(list) {				\
	int i;						\
	for (i = 0;list[i].key;i++)			\
		bind_key(NULL, list[i].key, find_variable(NULL, list[i].var), create_string(list[i].params));	\
}

#endif


