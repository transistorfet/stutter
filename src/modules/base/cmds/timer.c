/*
 * Command Name:	timer.c
 * Version:		0.1
 * Module Requirements:	utils ; string ; frontend ; timer
 * Description:		Set a timer event and evaluate the given command when
 *			it expires.
 */

#include <string.h>

#include CONFIG_H
#include <stutter/utils.h>
#include <stutter/macros.h>
#include <stutter/string.h>
#include <stutter/frontend.h>
#include <stutter/frontend/timer.h>

static int base_cmd_timer_expired(char *, fe_timer_t *);

int base_cmd_timer(char *env, char *args)
{
	int secs;
	char *time, *str;

	get_param_m(args, time, ' ');
	secs = util_atoi(time, 10);
	if (!(str = create_string("%s", args)) || !fe_timer_create(FE_TIMER_BF_ONE_TIME, secs, (callback_t) base_cmd_timer_expired, str))
		return(-1);
	return(0);
}


static int base_cmd_timer_expired(char *args, fe_timer_t *timer)
{
	char *cmd, *str;

	str = args;
	get_param_m(str, cmd, ' ');
	if (util_evaluate_command(cmd, str)) {
		BASE_ERROR_JOINPOINT(BASE_ERR_UNKNOWN_COMMAND, cmd)
	}
	destroy_string(args);
	return(0);
}



