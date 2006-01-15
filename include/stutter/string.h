/*
 * Header Name:		string.h
 * Version:		0.1
 * Description:		String Type Header
 */

#ifndef _LIB_STRING_H
#define _LIB_STRING_H

typedef char * string_t;

string_t create_string(char *, ...);
string_t create_empty_string(unsigned int);
void destroy_string(string_t);
string_t duplicate_string(string_t);

#endif

