/*
 * Header Name:		macros.h
 * Version:		0.1
 * Description:		Macro Definitions
 */

#ifndef _STUTTER_MACROS_H
#define _STUTTER_MACROS_H

#include <string.h>

/**
 * Calculates the address of an offset after a struct for when struct
 * data is allocated along with the struct.
 */
#define offset_after_struct_m(ptr, offset)	\
	(((size_t) (ptr)) + sizeof(*(ptr)) + (offset))

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
#define split_string_m(str, first, rest, ch) {		\
	(first) = (str);				\
	if (((rest) = strchr((str), (ch)))) {		\
		*(rest) = '\0';				\
		(rest) = &(rest)[1];			\
	}						\
	else						\
		(rest) = "";				\
}

/**
 * Assign the string from the start of str up to (but not including) the
 * first ch character to prefix and change str to point to the remaining
 * string after ch.  If the ch character is not found, prefix equals NULL
 * and str is unchanged.
 */
#define get_prefix_m(str, prefix, ch) {		\
	(prefix) = (str);			\
	if (((str) = strchr((str), (ch)))) {	\
		*(str) = '\0';			\
		(str) = &(str)[1];		\
	}					\
	else {					\
		(str) = (prefix);		\
		(prefix) = NULL;		\
	}					\
}

/**
 * Trim any whitespaces at the start of str by making str to point to the
 * first non-whitespace character in the string.  A whitespace character
 * is a space, tab, line feed, or carriage return.
 */
#define trim_whitespace_m(str)		\
	while ((*(str) == ' ') || (*(str) == '\t') || (*(str) == '\n') || (*(str) == '\r'))	\
		(str)++;

/**
 * Compare two strings case insensitively.
 */
#define strcmp_icase(str1, str2)	\
	strcasecmp(str1, str2)

/**
 * Compare two strings case insensitively up to a maximum number of characters.
 */
#define strncmp_icase(str1, str2, max)	\
	strncasecmp(str1, str2, max)

/**
 * Returns true is the given character is one of the ASCII char that represent
 * the numbers 0-9 or false otherwise.
 */
#define is_number_char_m(ch)	\
	( ((ch) >= 0x30) && ((ch) <= 0x39) )

/**
 * Copy the given string to the given buffer converting to lower case up to the
 * given max or until the given ch is reached.
 */
static inline int strcpy_lcase(char *str, char *buffer, int max, char ch)
{
	int i;

	for (i = 0;(str[i] != '\0') && (str[i] != ch);i++) {
		if ((str[i] >= 0x41) && (str[i] <= 0x5a))
			buffer[i] = str[i] + 0x20;
		else
			buffer[i] = str[i];
	}
	buffer[i++] = '\0';
	return(i);
}

#endif


