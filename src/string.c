/*
 * Module Name:		stringt.c
 * Description:		String Type
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <stutter/memory.h>
#include <stutter/string.h>

#define STRING_MAX_SIZE			1024

/**
 * Allocate a string big enough to hold the given string after
 * character expansion (using the given arguments) and return
 * it or NULL on error.
 */
string_t create_string(const char *fmt, ...)
{
	va_list va;
	string_t str;
	char buffer[STRING_MAX_SIZE];

	va_start(va, fmt);
	vsnprintf(buffer, STRING_MAX_SIZE, fmt, va);
	va_end(va);
	if (!(str = memory_alloc(strlen(buffer) + 1)))
		return(NULL);
	strcpy(str, buffer);
	return(str);
}

/**
 * Create an initially empty string of the given size and return it
 * or return NULL on error.
 */
string_t create_empty_string(unsigned int size)
{
	string_t str;

	if (!(str = memory_alloc(size + 1)))
		return(NULL);
	str[0] = '\0';
	return(str);
}

/**
 * Free the memory used by the given string.
 */
void destroy_string(string_t str)
{
	if (str)
		memory_free(str);
}

/**
 * Allocate a string and copy the given string to it.
 */
string_t duplicate_string(const string_t str)
{
	string_t dup;

	if (!str || !(dup = memory_alloc(strlen(str) + 1)))
		return(NULL);
	strcpy(dup, str);
	return(dup);
}

/**
 * Copy the given string to the given buffer up to the given
 * maximum number of characters - 1 (for terminator).  The
 * number of characters copied is returned or -1 on error.
 */
int copy_string(const string_t str, char *buffer, int max)
{
	if (!str)
		return(-1);
	strncpy(buffer, str, max - 1);
	buffer[max - 1] = '\0';
	return(strlen(buffer));
}


