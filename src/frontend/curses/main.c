/*
 * Module Name:		main.c
 * Version:		0.1
 * Module Requirements:	type ; signal ; string ; variable ; net ; terminal
 * Description:		Curses Frontend Main Loop
 */


#include <stdio.h>
#include <stdlib.h>

#include CONFIG_H
#include <stutter/init.h>
#include <stutter/type.h>
#include <stutter/signal.h>
#include <stutter/variable.h>
#include <stutter/frontend.h>
#include <stutter/lib/queue.h>
#include <stutter/lib/string.h>
#include <stutter/lib/globals.h>
#include <stutter/frontend/widget.h>
#include <stutter/frontend/common.h>

#include "core/desc.h"
#include "core/terminal.h"

static int handle_quit(char *, void *, char *);

DEFINE_TYPE_LIST(fe_types,
	ADD_TYPE(fe_common_load_colour)
	ADD_TYPE(fe_common_load_style)
	ADD_TYPE(fe_common_load_attrib)
	ADD_TYPE(fe_common_load_command)
);

DEFINE_HANDLER_LIST(fe_handlers,
	ADD_HANDLER("fe.quit", NULL, 0, handle_quit, NULL)
);

DEFINE_KEY_LIST(fe_keys,
	FE_BINDINGS()
);

DEFINE_VARIABLE_LIST(fe_variables,
	DECLARE_TYPE("attrib:fe",
		FE_COMMON_ATTRIBS()
	)
	DECLARE_TYPE("command:fe",
		FE_COMMON_COMMANDS()
	)
	DECLARE_TYPE("format",
		FE_COMMON_FORMATS()
	)
);

int exit_flag = 1;

extern int init_net(void);
extern int release_net(void);

extern int init_timer(void);
extern int release_timer(void);

extern int init_execute(void);
extern int release_execute(void);

extern int init_frontend(void);
extern int release_frontend(void);

extern int fe_timer_check(void);

struct variable_table_s *fe_table;
struct variable_table_s *fe_theme;

int init_curses(void)
{
	struct type_s *type;

	if (init_system())
		return(-1);

	add_signal("fe.quit", 0);
	ADD_TYPE_LIST(fe_types);
	ADD_HANDLER_LIST(fe_handlers);

	if (init_desc())
		return(-1);
	if (init_net())
		return(-1);
	if (init_timer())
		return(-1);
	if (init_execute())
		return(-1);

	#undef MODULE
	#define MODULE(name)	LOAD_MODULE(name)
	MODULE_LIST()

	if (!(type = find_type("table")) || !(fe_table = add_variable(NULL, type, "fe", 0, "")))
		return(-1);
	fe_theme = add_variable(fe_table, type, "theme", 0, "");

	ADD_VARIABLE_LIST(fe_table, fe_variables);
	ADD_KEY_LIST(fe_keys);

	if (init_terminal())
		return(-1);
	if (init_frontend())
		return(-1);
	return(0);
}

int release_curses(void)
{
	#undef MODULE
	#define MODULE(name)	RELEASE_MODULE(name)
	MODULE_LIST()

	release_frontend();
	release_terminal();
	release_execute();
	release_timer();
	release_net();
	release_desc();
	release_system();
	return(0);
}

/**
 * Main Entry Point
 */
main(int argc, char **argv)
{
	int ch;
	struct widget_s *input;

	if (init_curses()) {
		printf("Failed to initialize frontend\n");
		release_curses();
		return(0);
	}

	fe_refresh(NULL);
	terminal_refresh(terminal);
	#ifdef STUTTER_INIT
	STUTTER_INIT(argc, argv);
	#endif

	while (exit_flag) {
		fe_refresh();
		terminal_refresh(terminal);
		fe_desc_wait(1);
		fe_timer_check();
		if ((ch = terminal_read_char()) && process_key(ch) && (input = (struct widget_s *) fe_current_widget("input", NULL)))
			widget_control(input, WCC_PROCESS_CHAR, ch);
	}

	release_curses();
	return(0);
}

static int handle_quit(char *env, void *index, char *args)
{
	exit_flag = 0;
	return(SIGNAL_STOP_EMIT);
}

