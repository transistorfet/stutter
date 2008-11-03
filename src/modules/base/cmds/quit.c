/*
 * Command Name:	quit.c
 * Description:		Quit Command
 */

#include <stutter/signal.h>
#include <stutter/modules/base/base.h>

int base_cmd_quit(char *env, char *args)
{
	signal_named_emit(VARIABLE_S(signal_table), "fe.quit", args);
	return(0);
}


