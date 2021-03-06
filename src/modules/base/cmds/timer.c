/*
 * Command Name:	timer.c
 * Description:		Set a timer event and evaluate the given command when
 *			it expires.
 */

#include <string.h>

#include CONFIG_H
#include <stutter/utils.h>
#include <stutter/output.h>
#include <stutter/string.h>
#include <stutter/frontend/timer.h>

static int base_cmd_timer_expired(char *, fe_timer_t *);

int base_cmd_timer(char *env, char *args)
{
	int secs;
	int pos = 0;
	char *time, *str;

	time = util_get_arg(args, &pos);
	secs = util_atoi(time, 10);
	if (!(str = create_string("%s", &args[pos])) || !fe_timer_create(FE_TIMER_BF_ONE_TIME, secs, (callback_t) base_cmd_timer_expired, str))
		return(-1);
	return(0);
}


static int base_cmd_timer_expired(char *args, fe_timer_t *timer)
{
	char *cmd;
	int pos = 0;

	cmd = util_get_arg(args, &pos);
	if (util_evaluate_command(cmd, &args[pos])) {
		OUTPUT_ERROR(BASE_ERR_UNKNOWN_COMMAND, cmd);
	}
	destroy_string(args);
	return(0);
}



