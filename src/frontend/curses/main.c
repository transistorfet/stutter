/*
 * Module Name:		main.c
 * Description:		Curses Frontend Main Loop
 */


#include <stdio.h>
#include <stdlib.h>

#include CONFIG_H
#include <stutter/init.h>
#include <stutter/signal.h>
#include <stutter/string.h>
#include <stutter/object.h>
#include <stutter/globals.h>
#include <stutter/variable.h>
#include <stutter/frontend/common.h>
#include <stutter/frontend/frontend.h>
#include <stutter/frontend/common/widget.h>
#include <stutter/frontend/common/surface.h>
#include <stutter/frontend/curses/terminal.h>
#include <stutter/frontend/curses/interface.h>
#include <stutter/frontend/common/widgets/page.h>

static int handle_quit(char *, void *, char *);
static int add_builtin_layouts(void);

DEFINE_TYPE_LIST(types_list,
	//ADD_TYPE()
);

DEFINE_HANDLER_LIST(handlers_list,
	ADD_HANDLER("fe.quit", 0, handle_quit, NULL)
);

DEFINE_KEY_LIST(keys_list,
	//ADD_BINDINGS()
	//BIND_KEY("\n", "base.parse", "irc.activate")
	BIND_KEY("\n", "fe.parse", "base.evaluate")
	//BIND_KEY("\t", "irc.complete", NULL)
	//BIND_KEY("^c^x", "base.quit", NULL)
	//BIND_KEY("^k", "fe.insert", "\x03")
	//BIND_KEY("^p", "base.scroll", "1")
	//BIND_KEY("^q", "fe.previous", NULL)
	//BIND_KEY("^u", "base.clearline", NULL)
	//BIND_KEY("^x", "fe.next", NULL)
	//BIND_KEY("^y", "base.scroll", "-1")
);

DEFINE_VARIABLE_LIST(variables_list,
	DECLARE_TYPE("table",
		ADD_FIXED_VARIABLE("fe", "")
		ADD_FIXED_VARIABLE("fe.theme", "")
	)
	DECLARE_TYPE("string",
		ADD_VARIABLE("fe.type", "s", "curses")
	)
	DECLARE_TYPE("hashattrib:fe",
		ADD_FIXED_VARIABLE("fe.theme.nicktable", "")
	)
	//DECLARE_TYPE("command",
		//ADD_FIXED_VARIABLE("fe.insert", "fp", fe_cmd_insert, NULL)
		//ADD_FIXED_VARIABLE("fe.next", "fp", fe_cmd_next, NULL)
		//ADD_FIXED_VARIABLE("fe.previous", "fp", fe_cmd_previous, NULL)
	//)
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
	DECLARE_TYPE("format",
		ADD_VARIABLE("fe.statusbar", "s", "\022statusbar\022 $base.time $irc.current_nick on $irc.current_channel")
	)
);

DEFINE_COMMAND_LIST(execution_list,
	//ADD_INITS()
);

int exit_flag = 1;

extern int init_timer(void);
extern int release_timer(void);

extern int init_execute(void);
extern int release_execute(void);

extern int init_system(void);
extern int release_system(void);

extern int init_frontend(void);
extern int release_frontend(void);

extern int fe_timer_check(void);

struct variable_table_s *fe_theme;

int init_curses(void)
{
	/** Initialize the core (variables, signals, etc) */
	if (init_system())
		return(-1);

	add_signal(signal_table, "fe");
	add_signal(signal_table, "fe.quit");
	ADD_TYPE_LIST(types_list);

	if (init_interface())
		return(-1);
	if (init_timer())
		return(-1);
	if (init_execute())
		return(-1);

	INIT_MODULES();

	ADD_HANDLER_LIST(handlers_list);
	ADD_VARIABLE_LIST(NULL, variables_list);

	fe_theme = (struct variable_table_s *) find_variable(NULL, "fe.theme", NULL);
	if (init_common_frontend())
		return(-1);
	if (init_terminal())
		return(-1);
	add_builtin_layouts();

	ADD_KEY_LIST(keys_list);

	if (init_frontend())
		return(-1);
	return(0);
}

int release_curses(void)
{
	RELEASE_MODULES();

	release_frontend();
	release_terminal();
	release_common_frontend();
	release_execute();
	release_timer();
	release_interface();
	release_system();
	return(0);
}

/**
 * Main Entry Point
 */
int main(int argc, char **argv)
{
	if (init_curses()) {
		printf("Failed to initialize frontend\n");
		release_curses();
		return(0);
	}

	fe_surface_refresh(NULL);
//	EVALUATE_COMMAND_LIST(execution_list);

	while (exit_flag) {
		// TODO should this instead call terminal refresh directly?
		fe_surface_refresh(NULL);
		fe_interface_select(1);
		// TODO test this seperatly before enabling
		//fe_timer_check();
	}

	release_curses();
	return(0);
}

static int handle_quit(char *env, void *obj, char *args)
{
	exit_flag = 0;
	return(SIGNAL_STOP_EMIT);
}

// TODO this is here until you can decide on how layouts will be created
static int add_builtin_layouts(void)
{
	struct fe_layout *layout;

	layout = make_layout("frame", make_layout_property("id", "frame", NULL), NULL, NULL);
	layout->next = make_layout("statusbar", make_layout_property("id", "statusbar", make_layout_property("text", FE_DEFAULT_STATUSBAR, NULL)), NULL, NULL);
	layout->next->next = make_layout("input", make_layout_property("id", "input", NULL), NULL, NULL);

	add_layout("", "root", make_layout("terminal", NULL, make_layout("region", make_layout_property("id", "region", make_layout_property("width", "80", make_layout_property("height", "25", NULL))), layout, NULL), NULL));
	add_layout("", "text", make_layout("text", NULL, NULL, NULL));
	return(0);
}


