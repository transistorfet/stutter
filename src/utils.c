/*
 * Module Name:		utils.c
 * Version:		0.1
 * Module Requirements:	type ; string ; variable
 * Description:		Utilities
 */

#include <stdlib.h>

#include CONFIG_H
#include <stutter/type.h>
#include <stutter/signal.h>
#include <stutter/variable.h>
#include <stutter/lib/macros.h>
#include <stutter/lib/string.h>
#include <stutter/utils.h>

#define MAX_NAME		64

/**
 * Convert the charcter escape sequence (assuming str starts after the escape
 * character) and stores the character in buffer[0].  The number of characters
 * read as a sequence from str is returned
 */
int util_escape_char(char *str, char *buffer)
{
	if (*str == '\0')
		return(0);
	switch (str[0]) {
		case 'n':
			buffer[0] = '\n';
			break;
		case 'r':
			buffer[0] = '\r';
			break;
		case 't':
			buffer[0] = '\t';
			break;
		case 'e':
			buffer[0] = '\x1b';
			break;
		case 'x':
			if ((str[1] != '\0') && (str[2] != '\0'))
				buffer[0] = (str[1] - 0x30) * 0x10 + (str[2] - 0x30);
			return(3);
		default:
			buffer[0] = str[0];
			break;
	}
	return(1);
}

/**
 * Convert the given key sequence representation into the sequence of
 * actual characters expected from the terminal.
 */
int util_convert_key(char *str, char *buffer, int max)
{
	int i, j = 0;

	max--;
	for (i = 0;(str[i] != '\0') && (j < max);i++) {
		if (str[i] == '$')
			j += util_expand_variable(&str[i], &buffer[j], max - j + 1, &i);
		else if (str[i] == '\\') {
			i += util_escape_char(&str[++i], &buffer[j++]);
		}
		else if (str[i] == '^') {
			if ((str[++i] >= 0x41) && (str[i] <= 0x5f))
				buffer[j++] = str[i] - 0x40;
		}
		else
			buffer[j++] = str[i];
	}
	buffer[j] = '\0';
	return(j);
}

/**
 * Format a string using the given fmt string and place th resulting
 * string into the given buffer.  The number of characters written to
 * the buffer is returned.  If a $ is encountered, the substring up
 * to the next space is taken to be a variable name.  If the variable
 * name is enclosed in { }, then the variable name is take to be
 * up to the closing }.  The variable is found and the stringify
 * function is called on it.  The resulting string is copied into the
 * buffer.  If a & follows the $ then the stringified string is
 * recursively expanded.
 */
int util_expand_str(char *fmt, char *buffer, int max)
{
	int i;
	int j = 0;

	max--;
	for (i = 0;(fmt[i] != '\0') && (j < max);i++) {
		if (fmt[i] == '\\') {
			if (fmt[++i] == '\0')
				break;
			i += util_escape_char(&fmt[i], &buffer[j++]) - 1;
		}
		else if (fmt[i] == '$')
			j += util_expand_variable(&fmt[i], &buffer[j], max - j + 1, &i);
		else
			buffer[j++] = fmt[i];
	}
	buffer[j] = '\0';
	return(j);
}

/**
 * Expand the variable referenced by the given str and copy the stringified
 * value to the given buffer up to the max number of characters.  The number
 * of characters written to the buffer is returned.  If the given str_count
 * pointer is not NULL then the number of characters used as the variable
 * name in str is added to the value it points to.  The given str may
 * start with the '$' char or may start just after it.
 */
int util_expand_variable(char *str, char *buffer, int max, int *str_count)
{
	int k;
	char delim;
	char *name;
	int recurse;
	int i = 0, j = 0;
	char value[STRING_SIZE];

	if (str[i] == '$')
		i++;

	if (str[i] == '&') {
		recurse = 1;
		i++;
	}
	else
		recurse = 0;

	if (str[i] == '{') {
		delim = '}';
		i++;
	}
	else
		delim = ' ';

	name = buffer;
	for (k = 0;(str[i] != '\0') && (str[i] != delim) && is_variable_char_m(str[i]) && (k < max - 1);k++, i++)
		buffer[k] = str[i];
	buffer[k] = '\0';
	if (stringify_variable(NULL, name, value, STRING_SIZE) >= 0) {
		if (recurse)
			j = util_expand_str(value, buffer, max);
		else {
			strncpy(buffer, value, (max - 1));
			if ((j = strlen(value)) >= max)
				j = max - 1;
		}
	}
	if (delim != '}')
		i--;

	if (str_count)
		*str_count += i;
	buffer[j] = '\0';
	return(j);
}

/**
 * Format a string given the sprintf-style format string and optional arguments
 * and emit that string to the given signal.
 */
int util_emit_str(char *name, void *index, char *fmt, ...)
{
	va_list va;
	char buffer[STRING_SIZE];

	va_start(va, fmt);
	if (vsnprintf(buffer, STRING_SIZE, fmt, va) < 0)
		return(-1);
	return(signal_emit(name, index, buffer));
}

