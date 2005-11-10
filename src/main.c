/*
 * Module Name:		main.c
 * Version:		0.1
 * Module Requirements:	command ; variable ; modirc ; modbase
 * Description:		IRC Client
 */

#include CONFIG_H
//#include <command.h>
#include <type.h>
#include <frontend.h>
#include <variable.h>
#include <modules/irc.h>
#include <modules/base.h>

#include <nit/callback.h>

int init_system(void)
{
	void *window;

	init_key();
	init_variable();

	/* Types */
	base_load_command();

	/* Load Modules */
	// TODO init_base();
	init_irc();

	/* Setup Base Module */
	//fe_bind_key(NULL, '\n', (callback_t) COMMAND_PARSER, (void *) NULL);
	bind_key(NULL, "\n", create_callback(0, 0, NULL, (callback_t) COMMAND_PARSER, NULL), create_string(""));
	bind_key(NULL, "\x18", create_callback(0, 0, NULL, (callback_t) base_cmd_next, NULL), create_string(""));

	//add_variable(find_type("command"), "base", "next", create_callback(0, 0, NULL, base_cmd_next, NULL));
	base_add_command(BASE_NAMESPACE, "next", (callback_t) base_cmd_next, NULL);
	base_add_command(BASE_NAMESPACE, "previous", (callback_t) base_cmd_previous, NULL);
	base_add_command(BASE_NAMESPACE, "scroll", (callback_t) base_cmd_scroll, NULL);
	base_add_command(BASE_NAMESPACE, "get", (callback_t) base_cmd_get, NULL);
	base_add_command(BASE_NAMESPACE, "set", (callback_t) base_cmd_set, NULL);
	base_add_command(BASE_NAMESPACE, "remove", (callback_t) base_cmd_remove, NULL);

	/* Setup IRC Module */
	fe_register_widget("irc:statusbar", create_callback(0, 0, NULL, (callback_t) irc_status_bar, NULL));
	// TODO should this be completely replaced by the autodispatcher and if so, should load msgs here.
	irc_dispatch_callback(create_callback(0, 0, NULL, (callback_t) DISPATCH_CALLBACK, NULL));

	//fe_bind_key(']', base_key_cmd, "next");

	base_add_command(IRC_NAMESPACE, "join", (callback_t) irc_cmd_join, NULL);
	base_add_command(IRC_NAMESPACE, "leave", (callback_t) irc_cmd_leave, NULL);
	base_add_command(IRC_NAMESPACE, "quit", (callback_t) irc_cmd_quit, NULL);
	base_add_command(IRC_NAMESPACE, "msg", (callback_t) irc_cmd_msg, NULL);
	base_add_command(IRC_NAMESPACE, "me", (callback_t) irc_cmd_me, NULL);
	base_add_command(IRC_NAMESPACE, "nick", (callback_t) irc_cmd_nick, NULL);
	base_add_command(IRC_NAMESPACE, "notice", (callback_t) irc_cmd_notice, NULL);
	base_add_command(IRC_NAMESPACE, "say", (callback_t) irc_cmd_say, NULL);
	base_add_command(IRC_NAMESPACE, "", (callback_t) irc_cmd_say, NULL);
	//select_namespace(IRC_NAMESPACE);
	select_variable_path("irc;base");

	/* Setup HTTP Module */


	if (!(window = fe_create_widget("irc:window", fe_get_parent(fe_current_widget()))))
		return(-1);

	#ifdef GREET_MSG
	fe_print(window, create_string(GREET_MSG));
	#endif

	#ifdef IRC_DEFAULT_SERVER
	if (irc_connect(IRC_DEFAULT_SERVER, IRC_DEFAULT_PORT, IRC_DEFAULT_NICK, window))
		fe_print(window, create_string("Error connecting to server %s on port %d", IRC_DEFAULT_SERVER, IRC_DEFAULT_PORT));
	#endif

	return(0);
}


int release_system(void)
{
	/* Release Modules */
	release_irc();

	release_variable();
	release_key();
	return(0);
}



