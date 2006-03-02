/*
 * Module Name:		load.c
 * Version:		0.1
 * Module Requirements:	type ; string ; variable ; modbase
 * Description:		Command Parser
 */

#include <stdlib.h>

#include CONFIG_H
#include <stutter/type.h>
#include <stutter/variable.h>
#include <stutter/lib/string.h>
#include <stutter/lib/globals.h>
#include <stutter/modules/base.h>

struct command_prototype_s {
	char *name;
	callback_t func;
	void *ptr;
};

#define ADD_COMMAND(name, func, env)	\
	{ name, (callback_t) base_cmd_##func, env },

#define DECLARE_COMMAND(name)	\
	{ #name, (callback_t) base_cmd_##name, NULL },

static struct command_prototype_s base_commands[] = {
	BASE_COMMANDS()
	{ NULL, NULL, NULL }
};

int init_base(void)
{
	int i = 0;
	struct type_s *type;

	/* Load Status Type */
	if (!(type = base_load_status()) || !type->create)
		return(-1);

	/* Load Time Type */
	if (!(type = base_load_time()) || !type->create)
		return(-1);
	add_variable(type, BASE_NAMESPACE, "time", type->create("%s", BASE_TIME));
	add_variable(type, BASE_NAMESPACE, "date", type->create("%s", BASE_DATE));
	add_variable(type, BASE_NAMESPACE, "timestamp", type->create("%s", BASE_TIMESTAMP));

	/* Load Command Type */
	if (!(type = base_load_command()) || !type->create)
		return(-1);
	for (;base_commands[i].name;i++)
		add_variable(type, BASE_NAMESPACE, base_commands[i].name, type->create("%r%p", base_commands[i].func, base_commands[i].ptr));

	return(0);
}

int release_base(void)
{
	return(0);
}


