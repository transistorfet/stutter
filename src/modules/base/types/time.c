/*
 * Type Name:		time.c
 * Version:		0.1
 * Module Requirements:	type ; string ; modbase
 * Description:		Time Stamp Type
 */

#include <time.h>
#include <stdlib.h>

#include <type.h>
#include <lib/string.h>
#include <modules/base.h>

#define MAX_BUFFER		64

static string_t base_time_stringify(void *);

struct type_s *base_load_time(void)
{
	return(add_type("time", (create_t) create_string, (stringify_t) base_time_stringify, NULL, (destroy_t) destroy_string));
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

