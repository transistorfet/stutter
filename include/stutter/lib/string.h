/*
 * Header Name:		string.h
 * Version:		0.1
 * Description:		String Type Header
 */

#ifndef _STUTTER_LIB_STRING_H
#define _STUTTER_LIB_STRING_H

#include <stdarg.h>

typedef char * string_t;

string_t create_string(char *, ...);
string_t create_empty_string(unsigned int);
string_t recreate_string(string_t, char *, ...);
string_t recreate_string_real(string_t, char *, va_list);
void destroy_string(string_t);
string_t duplicate_string(string_t);

#endif

