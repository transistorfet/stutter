/*
 * Header Name:		string.h
 * Description:		String Type Header
 */

#ifndef _STUTTER_STRING_H
#define _STUTTER_STRING_H

#include <stdarg.h>

typedef char * string_t;

string_t create_string(const char *, ...);
string_t create_empty_string(unsigned int);
string_t recreate_string(string_t, const char *, ...);
string_t recreate_string_real(string_t, const char *, va_list);
string_t type_recreate_string(string_t, const char *, va_list);
void destroy_string(string_t);
string_t duplicate_string(const string_t);
int copy_string(const string_t, char *, int);

#endif

