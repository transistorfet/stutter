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
#include <stutter/frontend/frontend.h>
#include <stutter/frontend/common/common.h>
#include <stutter/frontend/common/widget.h>
#include <stutter/frontend/common/surface.h>
#include <stutter/frontend/curses/terminal.h>
#include <stutter/frontend/curses/interface.h>
#include <stutter/frontend/common/widgets/page.h>

static int handle_quit(char *, struct signal_s *, va_list);

DEFINE_TYPE_LIST(types_list,
	//ADD_TYPE()
);

DEFINE_HANDLER_LIST(handlers_list,
	ADD_HANDLER("fe.quit", 0, handle_quit, NULL)
	ADD_HANDLER("output.create", 10, fe_common_sig_create, NULL)
	ADD_HANDLER("output.destroy", 10, fe_common_sig_destroy, NULL)
	ADD_HANDLER("output.error", 10, fe_common_sig_print, NULL)
	ADD_HANDLER("output.status", 10, fe_common_sig_print, NULL)
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
	DECLARE_TYPE("command",
		ADD_FIXED_VARIABLE("fe.bind", "fp", fe_common_cmd_bind, NULL)
		ADD_FIXED_VARIABLE("fe.clear", "fp", fe_common_cmd_clear, NULL)
		ADD_FIXED_VARIABLE("fe.clearline", "fp", fe_common_cmd_clearline, NULL)
		ADD_FIXED_VARIABLE("fe.close", "fp", fe_common_cmd_close, NULL)
		ADD_FIXED_VARIABLE("fe.context", "fp", fe_common_cmd_context, NULL)
		ADD_FIXED_VARIABLE("fe.echo", "fp", fe_common_cmd_echo, NULL)
		ADD_FIXED_VARIABLE("fe.insert", "fp", fe_common_cmd_insert, NULL)
		ADD_FIXED_VARIABLE("fe.next", "fp", fe_common_cmd_next, NULL)
		ADD_FIXED_VARIABLE("fe.parse", "fp", fe_common_cmd_parse, NULL)
		ADD_FIXED_VARIABLE("fe.previous", "fp", fe_common_cmd_previous, NULL)
		ADD_FIXED_VARIABLE("fe.scroll", "fp", fe_common_cmd_scroll, NULL)
		ADD_FIXED_VARIABLE("fe.select", "fp", fe_common_cmd_select, NULL)
		ADD_FIXED_VARIABLE("fe.unbind", "fp", fe_common_cmd_unbind, NULL)
		ADD_FIXED_VARIABLE("fe.window", "fp", fe_common_cmd_window, NULL)
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

	add_signal(VARIABLE_S(signal_table), "fe");
	add_signal(VARIABLE_S(signal_table), "fe.quit");
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
	if (init_key())
		return(-1);
	if (init_widget())
		return(-1);
	if (init_surface())
		return(-1);
	if (init_terminal())
		return(-1);

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
	release_surface();
	release_widget();
	release_key();
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

static int handle_quit(char *env, struct signal_s *signal, va_list va)
{
	const char *text;

	text = va_arg(va, const char *);
	// TODO check that this is really a quit signal
	exit_flag = 0;
	return(SIGNAL_STOP_EMIT);
}


