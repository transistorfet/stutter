/*
 * Module Name:		init.c
 * Description:		Base Module Initializer
 */

#include <stdlib.h>

#include CONFIG_H
#include <stutter/init.h>
#include <stutter/signal.h>
#include <stutter/string.h>
#include <stutter/object.h>
#include <stutter/globals.h>
#include <stutter/variable.h>
#include <stutter/modules/base/base.h>

DEFINE_TYPE_LIST(types_list,
	ADD_TYPE(&base_command_type)
	ADD_TYPE(&base_format_type)
	ADD_TYPE(&base_status_type)
	ADD_TYPE(&base_string_type)
	ADD_TYPE(&base_time_type)
	ADD_TYPE(&base_wildcard_type)
);

DEFINE_VARIABLE_LIST(variables_list,
	DECLARE_TYPE("table",
		ADD_FIXED_VARIABLE("base", "")
	)
	DECLARE_TYPE("command",
		ADD_COMMAND_ENV("base.alias", base_cmd_alias, NULL)
		ADD_COMMAND_ENV("base.concat", base_cmd_concat, NULL)
		ADD_COMMAND_ENV("base.evaluate", base_cmd_evaluate, NULL)
		//ADD_COMMAND_ENV("base.exec", base_cmd_exec, NULL)
		ADD_COMMAND_ENV("base.expand", base_cmd_expand, NULL)
		ADD_COMMAND_ENV("base.get", base_cmd_get, NULL)
		//ADD_COMMAND_ENV("base.parse", base_cmd_parse, NULL)
		ADD_COMMAND_ENV("base.quit", base_cmd_quit, NULL)
		//ADD_COMMAND_ENV("base.redirect", base_cmd_redirect, NULL)
		ADD_COMMAND_ENV("base.remove", base_cmd_remove, NULL)
		ADD_COMMAND_ENV("base.set", base_cmd_set, NULL)
		ADD_COMMAND_ENV("base.source", base_cmd_source, NULL)
		ADD_COMMAND_ENV("base.timer", base_cmd_timer, NULL)
	)
	DECLARE_TYPE("time",
		ADD_VARIABLE("base.date", "s", "%d-%b")
		ADD_VARIABLE("base.time", "s", "%H:%M")
		ADD_VARIABLE("base.timestamp", "s", "%d-%b:%H:%M")
	)
	DECLARE_TYPE("wildcard",
		ADD_VARIABLE("path", "s", DEFAULT_PATH)
	)
);

int init_base(void)
{
	ADD_TYPE_LIST(types_list);

	ADD_VARIABLE_LIST(NULL, variables_list);
	return(0);
}

int release_base(void)
{
	struct object_type_s *type;

	/** Remove the whole variable table */
	if ((type = object_find_type("table", NULL))) {
		add_variable(NULL, type, "base", 0, "");
		remove_variable(NULL, type, "base");
	}
	return(0);
}


