/*
 * Command Name:	close.c
 * Description:		Close Window Command
 */

#include <stutter/macros.h>
#include <stutter/frontend.h>
#include <stutter/modules/base.h>

int base_cmd_close(char *env, char *args)
{
	char *name;
	void *window;

	get_param_m(args, name, ' ');
	if (*name != '\0') {
		if (!(window = fe_find_widget(name)))
			return(-1);
	}
	else if (!(window = fe_get_target(NULL, "text")))
		return(-1);
	fe_destroy_widget(window);
	return(0);
}


