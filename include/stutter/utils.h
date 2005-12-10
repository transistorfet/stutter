/*
 * Header Name:		utils.h
 * Version:		0.1
 * Description:		Utilities Header
 */

#ifndef _UTILITIES_H
#define _UTILITIES_H

#include <nit/string.h>

int util_escape_char(char *, char *);
string_t util_convert_key(char *);
string_t util_expand_str(char *);
int util_expand_to_buffer(char *, char *, int);

#endif


