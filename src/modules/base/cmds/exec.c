/*
 * Command Name:	exec.c
 * Version:		0.1
 * Module Requirements:	signal ; frontend ; modbase
 * Description:		Execute External Program Command
 */

#include CONFIG_H
#include <stutter/macros.h>
#include <stutter/signal.h>
#include <stutter/frontend.h>
#include <stutter/modules/base.h>
#include <stutter/frontend/execute.h>

static int base_cmd_exec_display(void *, fe_execute_t);

int base_cmd_exec(char *env, char *args)
{
	char *flags;
	void *window;
	fe_execute_t exec;

	if (!(window = fe_current_widget("text", NULL)))
		window = fe_first_widget("text", NULL);
	trim_whitespace_m(args);
	if (args[0] == '-') {
		get_param_m(args, flags, ' ');
		if (flags[1] == 'o')
			window = NULL;
	}

	if (!(exec = fe_execute_open(args, 0)))
		return(-1);
	fe_execute_set_callback(exec, IO_COND_READ, base_cmd_exec_display, window);
	return(0);
}

/*** Local Functions ***/

static int base_cmd_exec_display(void *window, fe_execute_t exec)
{
	int size;
	char buffer[STRING_SIZE];

	if ((size = fe_execute_receive_str(exec, buffer, STRING_SIZE, '\n')) < 0)
		fe_execute_close(exec);
	else if (size != 0) {
		buffer[size] = '\0';
		if (window)
			fe_print(window, buffer);
		else
			emit_signal(NULL, "base.exec_output", buffer);
	}
	return(0);
}

