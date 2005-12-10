/*
 * Module Name:		main.c
 * Version:		0.1
 * Module Requirements:	type ; utils ; variable ; frontend ; modirc ; modbase
 * Description:		IRC Client
 */

#include CONFIG_H
#include <type.h>
#include <utils.h>
#include <variable.h>
#include <frontend.h>
#include <modules/irc.h>
#include <modules/base.h>

#include <nit/string.h>

int init_system(void)
{
	init_key();
	init_variable();

	add_type("string", (create_t) create_string, (stringify_t) duplicate_string, NULL, (destroy_t) destroy_string);
	add_type("format", (create_t) create_string, (stringify_t) util_expand_str, NULL, (destroy_t) destroy_string);

	return(0);
}

int release_system(void)
{
	release_variable();
	release_key();
	return(0);
}

int load_modules(void)
{
	void *window;

	/* Load Modules */
	init_base();
	init_irc();

	select_variable_path(VARIABLE_PATH);

	/* Setup Base Module */
	bind_key(NULL, "\n", find_variable(NULL, NULL, "parse"), create_string(""));
	bind_key(NULL, "\x18", find_variable(NULL, NULL, "next"), create_string(""));
	bind_key(NULL, "\x11", find_variable(NULL, NULL, "previous"), create_string(""));

	#ifdef GREET_MSG
	fe_print(window, create_string(GREET_MSG));
	#endif

	#ifdef IRC_DEFAULT_SERVER
	if (!(window = fe_create_widget("irc", "window", NULL, NULL)))
		return(-1);
	if (!irc_server_connect(IRC_DEFAULT_SERVER, IRC_DEFAULT_PORT, IRC_DEFAULT_NICK, window))
		fe_print(window, create_string("Error connecting to server %s on port %d", IRC_DEFAULT_SERVER, IRC_DEFAULT_PORT));
	#endif

	return(0);
}

int unload_modules(void)
{
	/* Release Modules */
	release_irc();
	release_base();
	return(0);
}

