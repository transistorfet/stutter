/*
 * Module Name:		init.c
 * Version:		0.1
 * Module Requirements:	type ; signal ; string ; variable ; modbase
 * Description:		Base Module Initializer
 */

#include <stdlib.h>

#include CONFIG_H
#include <stutter/init.h>
#include <stutter/type.h>
#include <stutter/signal.h>
#include <stutter/variable.h>
#include <stutter/lib/string.h>
#include <stutter/lib/globals.h>
#include <stutter/modules/base.h>

/* Handlers List */

static struct handler_prototype_s base_handlers[] = {
	BASE_HANDLERS()
	{ NULL, NULL, 0, NULL, NULL }
};

/* Types List */

typedef int (*init_t)(void);

struct type_prototype_s {
	char *name;
	init_t func;
};

#define ADD_TYPE(name, func)	\
	{ name, func },

static struct type_prototype_s base_types[] = {
//	BASE_TYPES()
	{ NULL, NULL }
};

/* Commands List */

static struct command_prototype_s base_commands[] = {
	BASE_COMMANDS()
	{ NULL, NULL, NULL }
};

static struct variable_s *base_table;

int init_base(void)
{
	int i = 0;
	struct type_s *type;

	add_signal("base.error", 0);
	add_signal("base.exec_output", 0);

	/* Add Signal Handlers */
	for (i = 0;base_handlers[i].name;i++)
		signal_connect(base_handlers[i].name, base_handlers[i].index, base_handlers[i].priority, (signal_t) base_handlers[i].func, base_handlers[i].ptr);

	/* Load Table Type */
	if (!(type = base_load_table()))
		return(-1);
	if (!(base_table = add_variable(NULL, type, "base", VAR_BF_NO_REMOVE, "")))
		return(-1);

	/* Load String Type */
	if (!(type = base_load_string()))
		return(-1);

	/* Load Format Type */
	if (!(type = base_load_format()))
		return(-1);

	/* Load Status Type */
	if (!(type = base_load_status()))
		return(-1);

	/* Load Time Type */
	if (!(type = base_load_time()) || !type->create)
		return(-1);
	add_variable(NULL, type, "time", 0, "%s", BASE_TIME);
	add_variable(NULL, type, "date", 0, "%s", BASE_DATE);
	add_variable(NULL, type, "timestamp", 0, "%s", BASE_TIMESTAMP);

	if (!(type = base_load_wildcard()) || !type->create)
		return(-1);
	add_variable(NULL, type, "path", 0, "%s", ";base;irc");


	/* Load Command Type */
	if (!(type = base_load_command()) || !type->create)
		return(-1);
	for (i = 0;base_commands[i].name;i++)
		add_variable(base_table->value, type, base_commands[i].name, 0, "%r%p", base_commands[i].func, base_commands[i].ptr);

	return(0);
}

int release_base(void)
{
	int i = 0;

	/** Remove the whole variable table */
	base_table->bitflags &= ~VAR_BF_NO_REMOVE;
	remove_variable(NULL, NULL, "base");

	for (i = 0;base_handlers[i].name;i++)
		signal_disconnect(base_handlers[i].name, base_handlers[i].index, (signal_t) base_handlers[i].func, base_handlers[i].ptr);

	remove_signal("base.exec_output");
	remove_signal("base.error");
	return(0);
}


