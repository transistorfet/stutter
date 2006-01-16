/*
 * Module Name:		stringt.c
 * Version:		0.1
 * Module Requirements:	memory
 * Description:		String Type
 */

#include <stdarg.h>
#include <string.h>

#include <lib/memory.h>
#include <lib/string.h>

#define STRING_MAX_SIZE			1024

/**
 * Allocate a string big enough to hold the given string after
 * character expansion (using the given arguments) and return
 * it or NULL on error.
 */
string_t create_string(char *str, ...)
{
	va_list va;
	string_t new_str;
	char buffer[STRING_MAX_SIZE];

	va_start(va, str);
	vsprintf(buffer, str, va);
	va_end(va);
	if (!(new_str = memory_alloc(strlen(buffer) + 1)))
		return(NULL);
	strcpy(new_str, buffer);
	return(new_str);
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
string_t duplicate_string(string_t str)
{
	string_t dup;

	if (!(dup = memory_alloc(strlen(str) + 1)))
		return(NULL);
	strcpy(dup, str);
	return(dup);
}


