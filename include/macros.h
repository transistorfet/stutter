/*
 * Header Name:		macros.h
 * Version:		0.1
 * Description:		Macro Definitions
 */

#ifndef _MACROS_H
#define _MACROS_H

#include <string.h>

/**
 * Assign the string from the start of str up to (but not including) the
 * first ch character to substr and change str to point to the remaining
 * string after ch.  If the ch character is not found, substr equals str
 * and str equals "".
 */
#define get_param_m(str, substr, ch)		\
	split_string_m(str, substr, str, ch)

/**
 * Assign the string from the start of str up to (but not including) the
 * first ch character to first and make rest point to the remaining
 * string after ch.  If the ch character is not found, first equals str
 * and rest equals "".
 */
#define split_string_m(str, first, rest, ch) {	\
	first = str;				\
	if (rest = strchr(str, ch)) {		\
		*rest = '\0';			\
		rest = &rest[1];		\
	}					\
	else					\
		rest = "";			\
}

/**
 * Assign the string from the start of str up to (but not including) the
 * first ch character to prefix and change str to point to the remaining
 * string after ch.  If the ch character is not found, prefix equals NULL
 * and str is unchanged.
 */
#define get_prefix_m(str, prefix, ch) {		\
	prefix = str;				\
	if (str = strchr(str, ch)) {		\
		*str = '\0';			\
		str = &str[1];			\
	}					\
	else {					\
		str = prefix;			\
		prefix = NULL;			\
	}					\
}

/**
 * Trim any whitespaces at the start of str by making str to point to the
 * first non-whitespace character in the string.  A whitespace character
 * is a space, tab, line feed, or carriage return.
 */
#define trim_whitespace_m(str)		\
	while ((*str == ' ') || (*str == '\t') || (*str == '\n') || (*str == '\r'))	\
		str++;

#endif


