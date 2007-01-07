/*
 * Type Name:		time.c
 * Version:		0.1
 * Module Requirements:	type ; string ; modbase
 * Description:		Time Stamp Type
 */

#include <time.h>
#include <stdlib.h>

#include <stutter/type.h>
#include <stutter/string.h>
#include <stutter/modules/base.h>

static int base_time_stringify(void *, char *, int);

struct type_s *base_load_time(void)
{
	return(add_type(
		"time",
		0,
		(type_create_t) type_recreate_string,
		(type_destroy_t) destroy_string,
		NULL,
		NULL,
		NULL,
		NULL,
		(type_stringify_t) base_time_stringify,
		NULL
	));
}

/*** Local Functions ***/

static int base_time_stringify(void *ptr, char *buffer, int max)
{
	int i;
	time_t current_time;

	current_time = time(NULL);
	if (!(i = strftime(buffer, max, (char *) ptr, localtime(&current_time))))
		return(-1);
	return(i);
}

