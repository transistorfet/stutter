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
void destroy_string(string_t);
string_t duplicate_string(const string_t);
int copy_string(const string_t, char *, int);

#endif

