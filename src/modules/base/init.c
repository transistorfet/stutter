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
#include <stutter/string.h>
#include <stutter/globals.h>
#include <stutter/variable.h>
#include <stutter/modules/base.h>

DEFINE_HANDLER_LIST(base_handlers,
	BASE_HANDLERS()
);

DEFINE_TYPE_LIST(base_types,
	BASE_TYPES()
);

DEFINE_VARIABLE_LIST(base_variables,
	DECLARE_TYPE("time",
		ADD_FIXED_VARIABLE("time", "string", BASE_TIME_FORMAT)
		ADD_FIXED_VARIABLE("date", "string", BASE_DATE_FORMAT)
		ADD_FIXED_VARIABLE("timestamp", "string", BASE_TIMESTAMP_FORMAT)
	)
	DECLARE_TYPE("command",
		BASE_COMMANDS()
	)
);

DEFINE_KEY_LIST(base_keys,
	BASE_BINDINGS()
);

static void *base_table;

int init_base(void)
{
	struct type_s *type;

	add_signal(NULL, "base.error", 0);
	add_signal(NULL, "base.exec_output", 0);

	ADD_TYPE_LIST(base_types);
	ADD_HANDLER_LIST(base_handlers);

	if (!(type = find_type("table")) || !(base_table = add_variable(NULL, type, "base", VAR_BF_NO_REMOVE, "")))
		return(-1);

	if (!(type = find_type("wildcard")) || !type->create)
		return(-1);
	add_variable(NULL, type, PATH_VARIABLE_NAME, 0, "string", BASE_DEFAULT_PATH);

	ADD_VARIABLE_LIST(base_table, base_variables);
	ADD_KEY_LIST(base_keys);

	BASE_INIT_JOINPOINT(base_table)

	return(0);
}

int release_base(void)
{
	struct type_s *type;

	BASE_RELEASE_JOINPOINT(base_table)

	/** Remove the whole variable table */
	if ((type = find_type("table"))) {
		add_variable(NULL, type, "base", 0, "");
		remove_variable(NULL, type, "base");
	}

	REMOVE_HANDLER_LIST(base_handlers);

	remove_signal(NULL, "base.exec_output");
	remove_signal(NULL, "base.error");
	return(0);
}


