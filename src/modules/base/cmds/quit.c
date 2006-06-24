/*
 * Command Name:	quit.c
 * Version:		0.1
 * Module Requirements:	frontend ; modbase
 * Description:		Quit Command
 */

#include <stutter/frontend.h>
#include <stutter/modules/base.h>

int base_cmd_quit(char *env, char *args)
{
	fe_quit(args);
	return(0);
}


