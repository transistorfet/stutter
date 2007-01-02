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

#define LOAD_MODULE(name) {			\
	extern int init_##name(void);		\
	init_##name();				\
}

#define RELEASE_MODULE(name) {			\
	extern int release_##name(void);	\
	release_##name();			\
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
	void *value;					\
	struct type_s *type;				\
	for (i = 0;list[i].key;i++) {			\
		if (value = find_variable(NULL, list[i].var, &type))	\
			bind_key(NULL, list[i].key, value, type, list[i].params);	\
	}						\
}

/*** Variables ***/

#define INIT_ADD_VARIABLE		0x01
#define INIT_FIND_TYPE			0x02

struct variable_prototype_s {
	int type;
	char *name;
	int bitflags;
	char *params;
	void *ptrs[4];
};

#define ADD_VARIABLE(name, params, ...)			\
	{ INIT_ADD_VARIABLE, name, 0, params, { __VA_ARGS__ } },

#define ADD_CONSTANT(name, params, ...)			\
	{ INIT_ADD_VARIABLE, name, VAR_BF_NO_MODIFY | VAR_BF_NO_REMOVE, params, { __VA_ARGS__ } },

#define ADD_FIXED_VARIABLE(name, params, ...)		\
	{ INIT_ADD_VARIABLE, name, VAR_BF_NO_REMOVE, params, { __VA_ARGS__ } },

#define ADD_COMMAND(name, func)				\
	ADD_FIXED_VARIABLE(name, "callback,pointer", func, NULL)

#define ADD_COMMAND_ENV(name, func, env)		\
	ADD_FIXED_VARIABLE(name, "callback,pointer", func, env)

#define DECLARE_TYPE(type, list)			\
	{ INIT_FIND_TYPE, type, 0, NULL, NULL },	\
	list

#define DEFINE_VARIABLE_LIST(name, list)		\
	static struct variable_prototype_s name[] = {	\
		list					\
		{ 0, NULL, 0, NULL, NULL }		\
	}

#define ADD_VARIABLE_LIST(table, list) {				\
	int i;								\
	struct type_s *type;						\
									\
	for (i = 0;list[i].type;i++) {					\
		switch (list[i].type) {					\
			case INIT_FIND_TYPE: {				\
				if (!(type = find_type(list[i].name)) || !type->create) {	\
					while (list[++i].type != INIT_FIND_TYPE) ;		\
					i--;				\
				}					\
				break;					\
			}						\
			case INIT_ADD_VARIABLE: {			\
				add_variable(table, type, list[i].name, list[i].bitflags, list[i].params, list[i].ptrs[0], list[i].ptrs[1], list[i].ptrs[2], list[i].ptrs[3]);	\
				break;					\
			}						\
			default:					\
				break;					\
		}							\
	}								\
}

/*** Commands ***/

#define EVALUATE_COMMAND(cmd)		\
	cmd,

#define DEFINE_COMMAND_LIST(name, list)	\
	static char *name[] = {		\
		list			\
		NULL			\
	}

#define EVALUATE_COMMAND_LIST(list) {					\
	int i;								\
	char *str, *cmd;						\
	char buffer[STRING_SIZE];					\
	for (i = 0;list[i];i++) {					\
		strcpy(buffer, list[i]);				\
		str = buffer;						\
		if (!strncmp(str, BASE_COMMAND_PREFIX, strlen(BASE_COMMAND_PREFIX)))	\
			str = &str[strlen(BASE_COMMAND_PREFIX)];	\
		get_param_m(str, cmd, ' ');				\
		if (util_evaluate_command(cmd, str)) {			\
			BASE_ERROR_JOINPOINT(BASE_ERR_UNKNOWN_COMMAND, cmd)	\
		}							\
	}								\
}

#endif


