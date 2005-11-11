/*
 * Module Name:		main.c
 * Version:		0.1
 * Module Requirements:	type ; variable ; frontend ; modirc ; modbase
 * Description:		IRC Client
 */

#include CONFIG_H
#include <type.h>
#include <variable.h>
#include <frontend.h>
#include <modules/irc.h>
#include <modules/base.h>

#include <nit/callback.h>

int init_system(void)
{
	void *window;

	init_key();
	init_variable();

	/* Load Modules */
	init_base();
	init_irc();

	/* Setup Base Module */
	bind_key(NULL, "\n", create_callback(0, 0, NULL, (callback_t) COMMAND_PARSER, NULL), create_string(""));
	bind_key(NULL, "\x18", create_callback(0, 0, NULL, (callback_t) base_cmd_next, NULL), create_string(""));

	/* Setup IRC Module */
	fe_register_widget("irc:statusbar", create_callback(0, 0, NULL, (callback_t) irc_status_bar, NULL));

	select_variable_path("irc;base");
	if (!(window = fe_create_widget("irc:window", fe_get_parent(fe_current_widget()))))
		return(-1);

	#ifdef GREET_MSG
	fe_print(window, create_string(GREET_MSG));
	#endif

	#ifdef IRC_DEFAULT_SERVER
	if (!irc_server_connect(IRC_DEFAULT_SERVER, IRC_DEFAULT_PORT, IRC_DEFAULT_NICK, window))
		fe_print(window, create_string("Error connecting to server %s on port %d", IRC_DEFAULT_SERVER, IRC_DEFAULT_PORT));
	#endif

	return(0);
}


int release_system(void)
{
	/* Release Modules */
	release_irc();
	release_base();

	release_variable();
	release_key();
	return(0);
}



