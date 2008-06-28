/*
 * Command Name:	exec.c
 * Description:		Execute External Program Command
 */

#include CONFIG_H
#include <stutter/utils.h>
#include <stutter/macros.h>
#include <stutter/signal.h>
#include <stutter/frontend/execute.h>
#include <stutter/modules/base/base.h>

static int base_cmd_exec_display(void *, fe_execute_t);

int base_cmd_exec(char *env, char *args)
{
	int pos = 0;
	char *flags;
	void *window;
	fe_execute_t exec;

	window = fe_get_target(NULL, "text");
	TRIM_WHITESPACE(args);
	if (args[0] == '-') {
		flags = util_get_arg(args, &pos);
		if (flags[1] == 'o')
			window = NULL;
	}

	if (!(exec = fe_execute_open(&args[pos], 0)))
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
			// TODO how the hell do you actually do signals with putting them in the var tables
			;//emit_signal(NULL, "base.exec_output", buffer);
	}
	return(0);
}

