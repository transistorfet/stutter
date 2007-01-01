/*
 * Command Name:	previous.c
 * Version:		0.1
 * Module Requirements:	frontend ; modbase
 * Description:		Previous Window Command
 */

#include <stutter/frontend.h>
#include <stutter/modules/base.h>

int base_cmd_previous(char *env, char *args)
{
	void *window;

	if (window = fe_current_widget("frame", NULL)) {
		if (!fe_previous_widget("text", window))
			fe_last_widget("text", window);
	}
	return(0);
}


