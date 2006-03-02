/*
 * Header Name:		utils.h
 * Version:		0.1
 * Description:		Utilities Header
 */

#ifndef _STUTTER_UTILS_H
#define _STUTTER_UTILS_H

#include <stutter/lib/string.h>

int util_escape_char(char *, char *);
string_t util_convert_key(char *);
string_t util_expand_str(char *);
int util_expand_to_buffer(char *, char *, int);
string_t util_expand_variable(char *, int *);
int util_expand_variable_to_buffer(char *, char *, int, int *);

#endif


