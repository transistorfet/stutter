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

#include "net.h"
#include "terminal.h"

int exit_flag = 1;

extern int init_frontend(void);
extern int release_frontend(void);

struct variable_table_s *fe_table;
static int handle_quit(char *, void *, char *);

int init_curses(void)
{
	struct type_s *type;
	struct variable_s *var;

	if (init_system())
		return(-1);

	add_signal("quit", 0);
	signal_connect("quit", NULL, 0, (signal_t) handle_quit, NULL);

	LOAD_MODULES();
	if (!(type = find_type("table")) || !(var = add_variable(NULL, type, "fe", 0, "")))
		return(-1);
	fe_table = var->value;

	if (!(type = find_type("string")))
		return(-1);
	add_variable(fe_table, type, "bold", 0, "\x02");
	add_variable(fe_table, type, "normal", 0, "");
	add_variable(fe_table, type, "flash", 0, "");
	add_variable(fe_table, type, "inverse", 0, "\x16");
	add_variable(fe_table, type, "colour", 0, "\x03");

	if (type = find_type("format"))
		add_variable(NULL, type, "fe.statusbar", 0, "%s", FE_STATUSBAR);
	BIND_KEYS();

	if (init_net())
		return(-1);
	if (init_terminal())
		return(-1);
	if (init_frontend())
		return(-1);
	return(0);
}

int release_curses(void)
{
	RELEASE_MODULES();
	release_frontend();
	release_terminal();
	release_net();
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
		fe_net_wait(1);
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

