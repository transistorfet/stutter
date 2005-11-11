/*
 * Module Name:		load.c
 * Version:		0.1
 * Module Requirements:	type ; variable ; string ; modbase
 * Description:		Command Parser
 */

#include <stdlib.h>

#include CONFIG_H
#include <type.h>
#include <variable.h>
#include <nit/string.h>
#include <nit/callback.h>
#include <modules/base.h>

struct command_prototype_s {
	char *name;
	callback_t func;
	void *ptr;
};

static struct command_prototype_s base_commands[] = {
	{ "next", (callback_t) base_cmd_next, NULL },
	{ "previous", (callback_t) base_cmd_previous, NULL },
	{ "scroll", (callback_t) base_cmd_scroll, NULL },
	{ "get", (callback_t) base_cmd_get, NULL },
	{ "set", (callback_t) base_cmd_set, NULL },
	{ "remove", (callback_t) base_cmd_remove, NULL },
	{ NULL, NULL, NULL }
};

int init_base(void)
{
	int i = 0;
	struct type_s *type;

	base_load_command();

	if (!(type = find_type("command")) || !type->create)
		return(-1);

	for (;base_commands[i].name;i++)
		add_variable(type, BASE_NAMESPACE, base_commands[i].name, type->create("%r%p", base_commands[i].func, base_commands[i].ptr));

	return(0);
}

int release_base(void)
{
	return(0);
}


