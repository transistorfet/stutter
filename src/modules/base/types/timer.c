/*
 * Type Name:		timer.c
 * Description:		Periodic Timer Type
 */

#include <string.h>

#include CONFIG_H
#include <stutter/type.h>
#include <stutter/utils.h>
#include <stutter/macros.h>
#include <stutter/memory.h>
#include <stutter/modules/base.h>
#include <stutter/frontend/timer.h>

#define BT_MAX_NUMBER		64

struct base_timer_s {
	fe_timer_t timer;
	char *str;
};

static void *base_timer_create(void *, char *, va_list);
static void base_timer_destroy(void *);
static int base_timer_expired(struct base_timer_s *, fe_timer_t *);

struct type_s *base_load_timer(void)
{
	return(add_type(
		"timer",
		0,
		(type_create_t) base_timer_create,
		(type_destroy_t) base_timer_destroy,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL
	));
}

static void *base_timer_create(void *value, char *params, va_list va)
{
	char *str;
	int i, secs;
	struct base_timer_s *bt;
	char number[BT_MAX_NUMBER];

	if (value)
		base_timer_destroy(value);

	if (strcmp(params, "string"))
		return(NULL);
	str = va_arg(va, char *);
	for (i = 0;(str[i] != ' ') && (i < BT_MAX_NUMBER);i++)
		number[i] = str[i];
	if (i >= BT_MAX_NUMBER)
		return(NULL);
	number[i] = '\0';
	str = &str[i + 1];
	secs = util_atoi(number, 10);
	va_end(va);

	if (!(bt = (struct base_timer_s *) memory_alloc(sizeof(struct base_timer_s) + strlen(str) + 1)))
		return(NULL);
	if (!(bt->timer = fe_timer_create(FE_TIMER_BF_PERIODIC, secs, (callback_t) base_timer_expired, bt))) {
		memory_free(bt);
		return(NULL);
	}
	bt->str = (char *) offset_after_struct_m(bt, 0);
	strcpy(bt->str, str);
	return((void *) bt);
}

static void base_timer_destroy(void *value)
{
	fe_timer_destroy(((struct base_timer_s *) value)->timer);
	memory_free(value);
}


static int base_timer_expired(struct base_timer_s *bt, fe_timer_t *timer)
{
	char *cmd;
	int pos = 0;
	char buffer[STRING_SIZE];

	strncpy(buffer, bt->str, STRING_SIZE - 1);
	buffer[STRING_SIZE - 1] = '\0';
	cmd = util_get_arg(buffer, &pos);
	if (util_evaluate_command(cmd, &buffer[pos])) {
		BASE_ERROR_JOINPOINT(BASE_ERR_UNKNOWN_COMMAND, cmd)
	}
	return(0);
}


