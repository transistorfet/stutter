/*
 * Module Name:		main.c
 * Version:		0.1
 * Module Requirements:	command ; variable ; modirc ; modbase
 * Description:		IRC Client
 */

#include CONFIG_H
#include <command.h>
#include <frontend.h>
#include <variable.h>
#include <modules/irc.h>
#include <modules/base.h>

int init_system(void)
{
	void *window;

	init_variable();
	init_command();

	/* Load Modules */
	init_irc();

	/* Setup Base Module */
	fe_bind_key(NULL, '\n', COMMAND_PARSER, NULL);

	add_command("next", (callback_t) base_cmd_next, NULL);
	add_command("previous", (callback_t) base_cmd_previous, NULL);
	add_command("scroll", (callback_t) base_cmd_scroll, NULL);
	add_command("get", (callback_t) base_cmd_get, NULL);
	add_command("set", (callback_t) base_cmd_set, NULL);

	/* Setup IRC Module */
	fe_register_widget("irc:statusbar", create_callback(0, 0, NULL, (callback_t) irc_status_bar, NULL));
	// TODO should this be completely replaced by the autodispatcher and if so, should load msgs here.
	irc_dispatch_callback(create_callback(0, 0, NULL, (callback_t) DISPATCH_CALLBACK, NULL));

	//fe_bind_key(']', base_key_cmd, "next");

	add_command("join", (callback_t) irc_cmd_join, NULL);
	add_command("leave", (callback_t) irc_cmd_leave, NULL);
	add_command("quit", (callback_t) irc_cmd_quit, NULL);
	add_command("msg", (callback_t) irc_cmd_msg, NULL);
	add_command("me", (callback_t) irc_cmd_me, NULL);
	add_command("nick", (callback_t) irc_cmd_nick, NULL);
	add_command("notice", (callback_t) irc_cmd_notice, NULL);
	add_command("say", (callback_t) irc_cmd_say, NULL);
	add_command("", (callback_t) irc_cmd_say, NULL);

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

	release_command();
	release_variable();
	return(0);
}



