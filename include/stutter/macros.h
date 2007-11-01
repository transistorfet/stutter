/*
 * Header Name:		macros.h
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
 * Trim any whitespaces at the start of str by making str to point to the
 * first non-whitespace character in the string.  A whitespace character
 * is a space, tab, line feed, or carriage return.
 */
#define TRIM_WHITESPACE(str)		\
	while ((*(str) == ' ') || (*(str) == '\t') || (*(str) == '\n') || (*(str) == '\r'))	\
		(str)++;

/**
 * Convert every lower case character in the given string to it's equivallent
 * uppercase character.
 */
static inline void UPPERCASE(char *str) {
	int i;

	for (i = 0;str[i] != '\0';i++) {
		if ((str[i] >= 'a') && (str[i] <= 'z'))
			str[i] -= 0x20;
	}
}

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


