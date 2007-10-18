/*
 * Module Name:		main.c
 * Description:		Curses Frontend Main Loop
 */


#include <stdio.h>
#include <stdlib.h>

#include CONFIG_H
#include <stutter/init.h>
#include <stutter/type.h>
#include <stutter/queue.h>
#include <stutter/signal.h>
#include <stutter/string.h>
#include <stutter/globals.h>
#include <stutter/variable.h>
#include <stutter/frontend.h>
#include <stutter/frontend/widget.h>
#include <stutter/frontend/common.h>

#include "core/desc.h"

static int handle_quit(char *, void *, char *);

DEFINE_TYPE_LIST(types_list,
	ADD_TYPES()
);

DEFINE_HANDLER_LIST(handlers_list,
	ADD_HANDLER(NULL, "fe.quit", 0, handle_quit, NULL)
	ADD_HANDLERS()
);

DEFINE_KEY_LIST(keys_list,
	ADD_BINDINGS()
);

DEFINE_VARIABLE_LIST(variables_list,
	DECLARE_TYPE("table",
		ADD_TABLES()
	)
	DECLARE_TYPE("hashattrib:fe",
		ADD_FIXED_VARIABLE("theme.nicktable", "")
	)
	ADD_VARIABLES()
);

DEFINE_COMMAND_LIST(execution_list,
	ADD_INITS()
);

int exit_flag = 1;

extern int init_net(void);
extern int release_net(void);

extern int init_timer(void);
extern int release_timer(void);

extern int init_execute(void);
extern int release_execute(void);

extern int init_terminal(void);
extern int release_terminal(void);

extern int init_frontend(void);
extern int release_frontend(void);

extern int init_system(void);
extern int release_system(void);

extern int fe_timer_check(void);
extern int fe_refresh(void);

struct variable_table_s *fe_theme;

int init_curses(void)
{
	struct type_s *type;

	if (init_system())
		return(-1);

	add_signal(NULL, "fe.quit", 0);
	ADD_TYPE_LIST(types_list)
	ADD_HANDLER_LIST(handlers_list)

	if (init_desc())
		return(-1);
	if (init_net())
		return(-1);
	if (init_timer())
		return(-1);
	if (init_execute())
		return(-1);

	INIT_MODULES()

	ADD_VARIABLE_LIST(NULL, variables_list)
	ADD_KEY_LIST(keys_list)

	fe_theme = (struct variable_table_s *) find_variable(NULL, "fe.theme", &type);
	if (init_frontend())
		return(-1);
	return(0);
}

int release_curses(void)
{
	RELEASE_MODULES()

	release_frontend();
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
int main(int argc, char **argv)
{
	if (init_curses()) {
		printf("Failed to initialize frontend\n");
		release_curses();
		return(0);
	}

	fe_refresh();
	EVALUATE_COMMAND_LIST(execution_list)

	while (exit_flag) {
		fe_refresh();
		fe_desc_wait(1);
		fe_timer_check();
	}

	release_curses();
	return(0);
}

static int handle_quit(char *env, void *obj, char *args)
{
	exit_flag = 0;
	return(SIGNAL_STOP_EMIT);
}

