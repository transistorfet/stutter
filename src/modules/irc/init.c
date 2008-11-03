/*
 * Module Name:		init.c
 * Description:		IRC Module Initializer
 */

#include <stdlib.h>

#include CONFIG_H
#include <stutter/init.h>
#include <stutter/signal.h>
#include <stutter/string.h>
#include <stutter/object.h>
#include <stutter/globals.h>
#include <stutter/variable.h>
#include <stutter/modules/irc/irc.h>
#include <stutter/modules/base/base.h>

DEFINE_TYPE_LIST(types_list,
	//ADD_TYPE(&base_command_type)

);

DEFINE_VARIABLE_LIST(variables_list,
	DECLARE_TYPE("table",
		ADD_FIXED_VARIABLE("irc", "")
	)
	DECLARE_TYPE("command",
		ADD_COMMAND_ENV("irc.activate", base_cmd_evaluate, "irc.say")
		ADD_COMMAND_ENV("irc.ctcp", irc_cmd_ctcp, NULL)
		ADD_COMMAND_ENV("irc.disconnect", irc_cmd_disconnect, NULL)
		ADD_COMMAND_ENV("irc.endquery", irc_cmd_endquery, NULL)
		ADD_COMMAND_ENV("irc.join", irc_cmd_join, NULL)
		ADD_COMMAND_ENV("irc.kick", irc_cmd_kick, NULL)
		ADD_COMMAND_ENV("irc.leave", irc_cmd_leave, NULL)
		ADD_COMMAND_ENV("irc.me", irc_cmd_me, NULL)
		ADD_COMMAND_ENV("irc.mode", irc_cmd_mode, NULL)
		ADD_COMMAND_ENV("irc.msg", irc_cmd_msg, NULL)
		ADD_COMMAND_ENV("irc.names", irc_cmd_names, NULL)
		ADD_COMMAND_ENV("irc.nick", irc_cmd_nick, NULL)
		ADD_COMMAND_ENV("irc.notice", irc_cmd_notice, NULL)
		ADD_COMMAND_ENV("irc.part", irc_cmd_leave, NULL)
		ADD_COMMAND_ENV("irc.ping", irc_cmd_ping, NULL)
		ADD_COMMAND_ENV("irc.query", irc_cmd_query, NULL)
		ADD_COMMAND_ENV("irc.reconnect", irc_cmd_reconnect, NULL)
		ADD_COMMAND_ENV("irc.say", irc_cmd_say, NULL)
		ADD_COMMAND_ENV("irc.server", irc_cmd_server, NULL)
		ADD_COMMAND_ENV("irc.topic", irc_cmd_topic, NULL)
		ADD_COMMAND_ENV("irc.whois", irc_cmd_whois, NULL)
		ADD_COMMAND_ENV("irc.whowas", irc_cmd_whowas, NULL)
	)
		//ADD_COMMAND_ENV("irc.complete", irc_cmd_complete, NULL)

/*
	// TODO do you have a status type? should you keep these function names?
	DECLARE_TYPE("status",
		ADD_VARIABLE("irc.current_channel", "f", irc_stringify_channel, NULL)
		ADD_VARIABLE("irc.current_nick", "f", irc_stringify_nick, NULL)
	)
*/
);



int init_irc(void)
{
	init_irc_server();
	ADD_TYPE_LIST(types_list);
	ADD_VARIABLE_LIST(NULL, variables_list);
	return(0);
}

int release_irc(void)
{
	struct object_type_s *type;

	/** Remove the whole variable table */
	if ((type = object_find_type("table", NULL))) {
		add_variable(NULL, type, "irc", 0, "");
		remove_variable(NULL, type, "irc");
	}
	release_irc_server();
	return(0);
}


