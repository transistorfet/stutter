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

	if (!(type = find_type("command:fe")))
		return(-1);
	add_variable(fe_table, type, "insert", 0, "callback,widget", fe_common_cmd_insert, NULL);

	if (!(type = find_type("string")))
		return(-1);
	add_variable(fe_table, type, "colour_test", 0, "string", "\x03\x30K0 \x03\x31K1 \x03\x32K2 \x03\x33K3 \x03\x34K4 \x03\x35K5 \x03\x36K6 \x03\x37K7 \x03\x38K8 \x03\x39K9 \x03\x31\x30K10 \x03\x31\x31K11 \x03\x31\x32K12 \x03\x31\x33K13 \x03\x31\x34K14 \x03\x31\x35K15");
	add_variable(fe_table, type, "bold_test", 0, "string", "\x02\x03\x30K0 \x03\x31K1 \x03\x32K2 \x03\x33K3 \x03\x34K4 \x03\x35K5 \x03\x36K6 \x03\x37K7 \x03\x38K8 \x03\x39K9 \x03\x31\x30K10 \x03\x31\x31K11 \x03\x31\x32K12 \x03\x31\x33K13 \x03\x31\x34K14 \x03\x31\x35K15");

	if (!(type = find_type("attrib:fe")))
		return(-1);
	add_variable(fe_theme, type, "bracket", 0, "string", FE_THEME_BRACKET);
	add_variable(fe_theme, type, "channel", 0, "string", FE_THEME_CHANNEL);
	add_variable(fe_theme, type, "default", 0, "string", FE_THEME_DEFAULT);
	add_variable(fe_theme, type, "error", 0, "string", FE_THEME_ERROR);
	add_variable(fe_theme, type, "message", 0, "string", FE_THEME_MESSAGE);
	add_variable(fe_theme, type, "nick", 0, "string", FE_THEME_NICK);
	add_variable(fe_theme, type, "server", 0, "string", FE_THEME_SERVER);
	add_variable(fe_theme, type, "status", 0, "string", FE_THEME_STATUS);
	add_variable(fe_theme, type, "statusbar", 0, "string", FE_THEME_STATUSBAR);
	add_variable(fe_theme, type, "timestamp", 0, "string", FE_THEME_TIMESTAMP);
	add_variable(fe_theme, type, "topic", 0, "string", FE_THEME_TOPIC);

	if (type = find_type("format"))
		add_variable(fe_table, type, "statusbar", 0, "string", FE_STATUSBAR_DEFAULT);
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

