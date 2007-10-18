/*
 * Command Name:	quit.c
 * Description:		Quit Command
 */

#include <stutter/frontend.h>
#include <stutter/modules/base.h>

int base_cmd_quit(char *env, char *args)
{
	fe_quit(args);
	return(0);
}


