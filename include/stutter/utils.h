/*
 * Header Name:		utils.h
 * Version:		0.1
 * Description:		Utilities Header
 */

#ifndef _UTILS_H
#define _UTILS_H

#include <lib/string.h>

int util_escape_char(char *, char *);
string_t util_convert_key(char *);
string_t util_expand_str(char *);
int util_expand_to_buffer(char *, char *, int);
string_t util_expand_variable(char *, int *);
int util_expand_variable_to_buffer(char *, char *, int, int *);

#endif


