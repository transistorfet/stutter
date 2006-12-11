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

DEFINE_HANDLER_LIST(base_handlers,
	BASE_HANDLERS()
);

DEFINE_TYPE_LIST(base_types,
	BASE_TYPES()
);

DEFINE_COMMAND_LIST(base_commands,
	BASE_COMMANDS()
);

DEFINE_KEY_LIST(base_keys,
	BASE_BINDINGS()
);

static struct variable_s *base_table;

int init_base(void)
{
	int i = 0;
	struct type_s *type;

	add_signal("base.error", 0);
	add_signal("base.exec_output", 0);

	ADD_TYPE_LIST(base_types);
	ADD_HANDLER_LIST(base_handlers);

	if (!(type = find_type("table")) || !(base_table = add_variable(NULL, type, "base", VAR_BF_NO_REMOVE, "")))
		return(-1);

	if (!(type = find_type("time")) || !type->create)
		return(-1);
	add_variable(NULL, type, "time", 0, "%s", BASE_TIME_FORMAT);
	add_variable(NULL, type, "date", 0, "%s", BASE_DATE_FORMAT);
	add_variable(NULL, type, "timestamp", 0, "%s", BASE_TIMESTAMP_FORMAT);

	if (!(type = find_type("wildcard")) || !type->create)
		return(-1);
	add_variable(NULL, type, PATH_VARIABLE_NAME, 0, "%s", BASE_DEFAULT_PATH);

	ADD_COMMAND_LIST(base_table, base_commands);
	ADD_KEY_LIST(base_keys);

	BASE_INIT_JOINPOINT(base_table)

	return(0);
}

int release_base(void)
{
	int i = 0;

	BASE_RELEASE_JOINPOINT(base_table)

	/** Remove the whole variable table */
	base_table->bitflags &= ~VAR_BF_NO_REMOVE;
	remove_variable(NULL, NULL, "base");

	REMOVE_HANDLER_LIST(base_handlers);

	remove_signal("base.exec_output");
	remove_signal("base.error");
	return(0);
}


