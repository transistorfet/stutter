/*
 * Header Name:		utils.h
 * Version:		0.1
 * Description:		Utilities Header
 */

#ifndef _STUTTER_UTILS_H
#define _STUTTER_UTILS_H

#include <stutter/lib/string.h>

int util_escape_char(char *, char *);
int util_convert_key(char *, char *, int);
int util_expand_str(char *, char *, int);
int util_expand_variable(char *, char *, int, int *);

#endif


