/*
 * Command Name:	quit.c
 * Description:		Quit Command
 */

#include <stutter/signal.h>
#include <stutter/modules/base.h>

int base_cmd_quit(char *env, char *args)
{
	signal_named_emit(signal_table, "fe.quit", args);
	return(0);
}


