/*
 * Module Name:		init.c
 * Description:		Common Frontend Initializer
 */

#include <stdlib.h>

#include CONFIG_H
#include <stutter/init.h>
#include <stutter/signal.h>
#include <stutter/string.h>
#include <stutter/object.h>
#include <stutter/globals.h>
#include <stutter/variable.h>
#include <stutter/frontend/common.h>
#include <stutter/frontend/common/layout.h>
#include <stutter/frontend/common/widget.h>

DEFINE_TYPE_LIST(types_list,
	// TODO add types
	//ADD_TYPE(&base_wildcard_type)
);

DEFINE_VARIABLE_LIST(variables_list,
	DECLARE_TYPE("command",
		ADD_COMMAND_ENV("fe.echo", fe_common_cmd_echo, NULL)
		ADD_COMMAND_ENV("fe.parse", fe_common_cmd_parse, NULL)
	)
/*
	DECLARE_TYPE("table",
		ADD_FIXED_VARIABLE("fe", "")
		ADD_FIXED_VARIABLE("fe.theme", "")
	)
	DECLARE_TYPE("hashattrib:fe",
		ADD_FIXED_VARIABLE("theme.nicktable", "")
	)
	DECLARE_TYPE("attrib:fe",
		ADD_VARIABLE("fe.theme.bracket", "s", ",,bold")
		ADD_VARIABLE("fe.theme.channel", "s", "default")
		ADD_VARIABLE("fe.theme.default", "s", "default")
		ADD_VARIABLE("fe.theme.error", "s", "default")
		ADD_VARIABLE("fe.theme.message", "s", "default")
		ADD_VARIABLE("fe.theme.nick", "s", "default")
		ADD_VARIABLE("fe.theme.server", "s", "default")
		ADD_VARIABLE("fe.theme.status", "s", "default")
		ADD_VARIABLE("fe.theme.statusbar", "s", ",,inverse")
		ADD_VARIABLE("fe.theme.timestamp", "s", "default")
		ADD_VARIABLE("fe.theme.topic", "s", "default")
	)
*/
);

int init_common_frontend(void)
{
	if (init_key())
		return(-1);
	if (init_widget())
		return(-1);
	if (init_surface())
		return(-1);
	if (init_layout())
		return(-1);

	// TODO this should be moved someplace else
	layout_register_type("window", LAYOUT_RT_WIDGET, (fe_layout_create_t) fe_widget_generate, &fe_window_type);
	layout_register_type("text", LAYOUT_RT_WIDGET, (fe_layout_create_t) fe_widget_generate, &fe_text_type);
	layout_register_type("frame", LAYOUT_RT_WIDGET, (fe_layout_create_t) fe_widget_generate, &fe_frame_type);
	layout_register_type("input", LAYOUT_RT_WIDGET, (fe_layout_create_t) fe_widget_generate, &fe_input_type);
	layout_register_type("statusbar", LAYOUT_RT_WIDGET, (fe_layout_create_t) fe_widget_generate, &fe_statusbar_type);
	layout_register_type("region", LAYOUT_RT_WIDGET, (fe_layout_create_t) fe_widget_generate, &fe_region_type);

	ADD_TYPE_LIST(types_list);
	ADD_VARIABLE_LIST(NULL, variables_list);
	return(0);
}

int release_common_frontend(void)
{
	release_layout();
	release_surface();
	release_widget();
	release_key();
	return(0);
}


