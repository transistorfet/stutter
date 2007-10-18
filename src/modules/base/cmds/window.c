/*
 * Command Name:	window.c
 * Description:		New Window Command
 */


#include <stutter/frontend.h>
#include <stutter/modules/base.h>

int base_cmd_window(char *env, char *args)
{
	void *frame, *window;

	if ((frame = fe_get_target(NULL, "frame")) && (window = fe_create_widget("base", "text", args, frame)))
		fe_show_widget(window);
	return(0);
}


