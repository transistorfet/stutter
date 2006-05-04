/*
 * Type Name:		time.c
 * Version:		0.1
 * Module Requirements:	type ; string ; modbase
 * Description:		Time Stamp Type
 */

#include <time.h>
#include <stdlib.h>

#include <stutter/type.h>
#include <stutter/lib/string.h>
#include <stutter/modules/base.h>

#define MAX_BUFFER		64

static string_t base_time_stringify(void *);

struct type_s *base_load_time(void)
{
	return(add_type(
		"time",
		0,
		(type_create_t) recreate_string_real,
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

static string_t base_time_stringify(void *ptr)
{
	time_t current_time;
	char buffer[MAX_BUFFER];

	current_time = time(NULL);
	strftime(buffer, MAX_BUFFER, (char *) ptr, localtime(&current_time));
	return(create_string("%s", buffer));
}

