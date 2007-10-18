/*
 * Command Name:	play.c
 * System Requirements:	Windows
 * Description:		Play Sound Command
 */

#include <windows.h>
#include <mmsystem.h>

#include <stutter/macros.h>
#include <stutter/frontend.h>

int fe_win32_cmd_play(char *env, char *args)
{
	char *name;

	get_param_m(args, name, ' ');
	PlaySound(name, NULL, SND_ASYNC | SND_FILENAME);
	return(0);
}


