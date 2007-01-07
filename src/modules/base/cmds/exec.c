/*
 * Command Name:	exec.c
 * Version:		0.1
 * Module Requirements:	frontend ; modbase
 * Description:		Execute External Program Command
 */

#include CONFIG_H
#include <stutter/macros.h>
#include <stutter/frontend.h>
#include <stutter/modules/base.h>
#include <stutter/frontend/execute.h>

static int base_cmd_exec_display(void *, fe_execute_t, void *);

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
	signal_connect("fe.read_ready", exec, 10, base_cmd_exec_display, window);
	return(0);
}

/*** Local Functions ***/

static int base_cmd_exec_display(void *window, fe_execute_t exec, void *ptr)
{
	int size;
	char buffer[STRING_SIZE];

	if ((size = fe_execute_receive_str(exec, buffer, STRING_SIZE, '\n')) < 0) {
		fe_execute_close(exec);
		signal_disconnect("fe.read_ready", exec, NULL, NULL);
	}
	else if (size != 0) {
		buffer[size] = '\0';
		if (window)
			fe_print(window, buffer);
		else
			emit_signal("base.exec_output", NULL, buffer);
	}
	return(0);
}

