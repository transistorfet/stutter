/*
 * Header Name:		init.h
 * Description:		Initialization Structures and Macros
 */

#ifndef _STUTTER_INIT_H
#define _STUTTER_INIT_H

#include <stutter/utils.h>
#include <stutter/object.h>
#include <stutter/output.h>
#include <stutter/signal.h>
#include <stutter/macros.h>
#include <stutter/variable.h>
#include <stutter/frontend/common/key.h>

/*** Types ***/

struct type_prototype_s {
	struct object_type_s *type;
};

#define ADD_TYPE(type)	\
	{ OBJECT_TYPE_S(type) },

#define DEFINE_TYPE_LIST(name, list)			\
	static struct type_prototype_s name[] = {	\
		list					\
		{ NULL }				\
	}

static inline void ADD_TYPE_LIST(struct type_prototype_s *list) {
	int i;
	for (i = 0;list[i].type;i++)
		object_register_type(list[i].type);
}

/*** Signals ***/

struct handler_prototype_s {
	char *name;
	int priority;
	signal_t func;
	void *ptr;
};

#define ADD_HANDLER(name, priority, func, env)	\
	{ name, priority, (signal_t) func, env },

#define DEFINE_HANDLER_LIST(name, list)			\
	static struct handler_prototype_s name[] = {	\
		list					\
		{ NULL, 0, NULL, NULL }			\
	}

static inline void ADD_HANDLER_LIST(struct handler_prototype_s *list) {
	int i;
	for (i = 0; list[i].name; i++)
		signal_named_connect(signal_table, list[i].name, list[i].priority, (signal_t) list[i].func, list[i].ptr);
}

static inline void REMOVE_HANDLER_LIST(struct handler_prototype_s *list) {
	int i;
	for (i = 0; list[i].name; i++)
		// TODO fix this somehow...
		;//signal_disconnect(signal_find_handler(SIGNAL_S(find_variable(NULL, list[i].name, NULL)), (signal_t) list[i].func, list[i].ptr));
}

/*** Keys ***/

struct key_prototype_s {
	char *context;
	char *key;
	char *var;
	char *params;
};

#define BIND_KEY(key, var, params)	\
	{ NULL, key, var, params },

#define DEFINE_KEY_LIST(name, list)			\
	static struct key_prototype_s name[] = {	\
		list					\
		{ NULL, NULL, NULL, NULL }		\
	}

static inline void ADD_KEY_LIST(struct key_prototype_s *list) {
	int i;
	struct variable_s *var;
	int key[SMALL_STRING_SIZE];
	for (i = 0; list[i].key; i++) {
		if ((var = find_variable(NULL, list[i].var, NULL))) {
			util_convert_key(list[i].key, key, SMALL_STRING_SIZE);
			fe_key_bind(list[i].context, key, var, list[i].params);
		}
	}
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
	ADD_FIXED_VARIABLE(name, "fp", func, NULL)

#define ADD_COMMAND_ENV(name, func, env)		\
	ADD_FIXED_VARIABLE(name, "fp", func, env)

#define DECLARE_TYPE(type, list)			\
	{ INIT_FIND_TYPE, type, 0, NULL, { NULL } },	\
	list

#define DEFINE_VARIABLE_LIST(name, list)		\
	static struct variable_prototype_s name[] = {	\
		list					\
		{ 0, NULL, 0, NULL, { NULL } }		\
	}

static inline void ADD_VARIABLE_LIST(struct variable_table_s *table, struct variable_prototype_s *list) {
	int i;
	struct object_type_s *type;

	for (i = 0; list[i].type; i++) {
		switch (list[i].type) {
			case INIT_FIND_TYPE: {
				if (!(type = object_find_type(list[i].name, NULL))) {
					for (i++; list[i].type && list[i].type != INIT_FIND_TYPE; i++)
						;
					i--;
				}
				break;
			}
			case INIT_ADD_VARIABLE: {
				add_variable(table, type, list[i].name, list[i].bitflags, list[i].params, list[i].ptrs[0], list[i].ptrs[1], list[i].ptrs[2], list[i].ptrs[3]);
				break;
			}
			default:
				break;
		}
	}
}

/*** Commands ***/

#define EVALUATE_COMMAND(cmd)		\
	cmd,

#define DEFINE_COMMAND_LIST(name, list)	\
	static char *name[] = {		\
		list			\
		NULL			\
	}

static inline void EVALUATE_COMMAND_LIST(const char **list) {
	int i, pos;
	char *str, *cmd;
	char buffer[STRING_SIZE];
	for (i = 0; list[i]; i++) {
		strcpy(buffer, list[i]);
		str = buffer;
		if (!strncmp(str, BASE_COMMAND_PREFIX, strlen(BASE_COMMAND_PREFIX)))
			str = &str[strlen(BASE_COMMAND_PREFIX)];
		pos = 0;
		cmd = util_get_arg(str, &pos);
		if (util_evaluate_command(cmd, &str[pos]))
			OUTPUT_ERROR(BASE_ERR_UNKNOWN_COMMAND, cmd);
	}
}

#endif


