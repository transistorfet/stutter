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

#include "desc.h"
#include "terminal.h"

int exit_flag = 1;

extern int init_net(void);
extern int release_net(void);

extern int init_execute(void);
extern int release_execute(void);

extern int init_frontend(void);
extern int release_frontend(void);

// TODO put these in an include file somewhere
extern struct type_s *fe_common_load_command(void);
extern int common_cmd_insert(struct widget_s *, char *);

struct variable_table_s *fe_table;
static int handle_quit(char *, void *, char *);

int init_curses(void)
{
	struct type_s *type;
	struct variable_s *var;

	if (init_system())
		return(-1);

	add_signal("fe.quit", 0);
	signal_connect("fe.quit", NULL, 0, (signal_t) handle_quit, NULL);

	LOAD_MODULES();
	if (!(type = find_type("table")) || !(var = add_variable(NULL, type, "fe", 0, "")))
		return(-1);
	fe_table = var->value;

	if (!(type = fe_common_load_command()))
		return(-1);
	add_variable(fe_table, type, "insert", 0, "(widget * string -> int) * widget", common_cmd_insert, NULL);

	if (!(type = find_type("string")))
		return(-1);
	add_variable(fe_table, type, "bold", 0, "\x02");
	add_variable(fe_table, type, "normal", 0, "");
	add_variable(fe_table, type, "flash", 0, "");
	add_variable(fe_table, type, "inverse", 0, "\x16");
	add_variable(fe_table, type, "colour", 0, "\x03");

	add_variable(fe_table, type, "colour_test", 0, "\x03\x30K0 \x03\x31K1 \x03\x32K2 \x03\x33K3 \x03\x34K4 \x03\x35K5 \x03\x36K6 \x03\x37K7 \x03\x38K8 \x03\x39K9 \x03\x31\x30K10 \x03\x31\x31K11 \x03\x31\x32K12 \x03\x31\x33K13 \x03\x31\x34K14 \x03\x31\x35K15");
	add_variable(fe_table, type, "bold_test", 0, "\x02\x03\x30K0 \x03\x31K1 \x03\x32K2 \x03\x33K3 \x03\x34K4 \x03\x35K5 \x03\x36K6 \x03\x37K7 \x03\x38K8 \x03\x39K9 \x03\x31\x30K10 \x03\x31\x31K11 \x03\x31\x32K12 \x03\x31\x33K13 \x03\x31\x34K14 \x03\x31\x35K15");

	if (type = find_type("format"))
		add_variable(fe_table, type, "statusbar", 0, "%s", FE_STATUSBAR_DEFAULT);

	if (init_desc())
		return(-1);
	if (init_net())
		return(-1);
	if (init_execute())
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
	release_execute();
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

