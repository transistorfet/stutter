/*
 * Command Name:	close.c
 * Description:		Close Window Command
 */

#include <stutter/utils.h>
#include <stutter/frontend/frontend.h>

int fe_common_cmd_close(char *env, char *args)
{
	char *name;
	void *window;

	name = util_get_arg(args, NULL);
	if (*name != '\0') {
		if (!(window = fe_find_widget(name)))
			return(-1);
	}
	else if (!(window = fe_get_target(NULL, "text")))
		return(-1);
	fe_destroy_widget(window);
	return(0);
}

