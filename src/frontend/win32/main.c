/*
 * Module Name:		main.c
 * System Requirements:	Windows OS
 * Description:		Windows Frontend Main Loop
 */


#include <windows.h>

#include CONFIG_H
#include <stutter/key.h>
#include <stutter/init.h>
#include <stutter/type.h>
#include <stutter/signal.h>
#include <stutter/string.h>
#include <stutter/globals.h>
#include <stutter/variable.h>
#include <stutter/frontend/widget.h>
#include <stutter/frontend/common.h>
#include "win32.h"

int handle_quit(char *, void *, char *);

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

DEFINE_VARIABLE_LIST(fe_variables,
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

HINSTANCE this_instance;
struct variable_table_s *fe_theme;

extern int init_net(void);
extern int release_net(void);

extern int init_execute(void);
extern int release_execute(void);

extern int init_timer(void);
extern int release_timer(void);

extern int init_frontend(void);
extern int release_frontend(void);

extern int init_system(void);
extern int release_system(void);

int init_windows(void)
{
	struct type_s *type;

	if (init_system()) 
		return(-1);

	add_signal(NULL, "fe.quit", 0);
	ADD_TYPE_LIST(types_list)

	if (init_net())
		return(-1);
	if (init_execute())
		return(-1);
	if (init_timer())
		return(-1);

	INIT_MODULES()

	ADD_HANDLER_LIST(handlers_list)
	ADD_VARIABLE_LIST(NULL, variables_list)
	ADD_KEY_LIST(keys_list)

	fe_theme = (struct variable_table_s *) find_variable(NULL, "fe.theme", &type);
	if (init_frontend()) 
		return(-1);
	return(0);
}

int release_windows(void)
{
	RELEASE_MODULES()

	release_frontend();
	release_timer();
	release_execute();
	release_net();
	release_system();
	return(0);
}

/**
 * Main Entry Point
 */
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR argv, int WinMode)
{
	MSG msg;

	/*** Initialization ***/
	this_instance = hInst;

	if (init_windows()) {
		MessageBox(NULL, "Failed to initialize system", NULL, MB_OK);
		release_windows();
		return(0);
	}

	EVALUATE_COMMAND_LIST(execution_list)

	/*** Main Loop ***/
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	release_windows();
	return(msg.wParam);
}

/*** Local Functions ***/

int handle_quit(char *env, void *obj, char *msg)
{
	PostQuitMessage(0);
	return(SIGNAL_STOP_EMIT);
}


