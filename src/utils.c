/*
 * Module Name:		utils.c
 * Description:		Utilities
 */

#include <stdlib.h>

#include CONFIG_H
#include <stutter/type.h>
#include <stutter/signal.h>
#include <stutter/macros.h>
#include <stutter/string.h>
#include <stutter/variable.h>
#include <stutter/utils.h>

#define MAX_NAME		64

/**
 * Convert a string of the given radix to an interger.
 */
int util_atoi(const char *str, int radix)
{
	int i = -1, ret = 0, mul = 1;

	if (!str)
		return(0);

	while (str[++i] == ' ' || str[i] == '\t' || str[i] == '\n')
		if (str[i] == '\0')
			return(0);

	if (str[i] == '-') {
		mul = -1;
		i++;
	}

	for (;str[i] != '\0';i++) {
		ret *= radix;
		if (str[i] >= 0x30 && str[i] <= 0x39)
			ret += (str[i] - 0x30);
		else if (str[i] >= 0x41 && str[i] <= 0x5a)
			ret += (str[i] - 0x37);
		else if (str[i] >= 0x61 && str[i] <= 0x7a)
			ret += (str[i] - 0x57);
		else
			break;
	}
	return(ret * mul);
}

/**
 * Convert a number to a string with the given radix.  The number of
 * characters written into the string is returned.
 */
int util_itoa(int num, char *str, int radix)
{
	int i = -1, mul;

	if (!str)
		return(0);

	if ((radix == 10) && (num < 0)) {
		str[++i] = '-';
		num *= -1;
	}

	mul = radix;
	while ((num / mul) != 0)
		mul *= radix;
	mul /= radix;

	do {
		str[++i] = (num / mul) + 0x30;
		if (str[i] > 0x39)
			str[i] += 0x07;
		num -= (num / mul) * mul;
		mul /= radix;
	} while (mul != 0);
	str[++i] = '\0';

	return(i);
}

/**
 * Encode the given data of the given size into the given character buffer in
 * an ASCII format up to the given maxiumum number of characters.  If the data
 * cannot be encoded within the given max size of characters then -1 will be
 * returned.  Otherwise the number of characters written to the buffer is
 * returned.
 */
int util_encode_bytes(unsigned char *data, int size, char *buffer, int max)
{
	int i, j;

	if (size * 2 > max)
		return(-1);
	for (i = 0, j = 0;i < size;i++, j++) {
		buffer[j] = ((data[i] & 0xf0) >> 4) + 0x30;
		buffer[++j] = (data[i] & 0x0f) + 0x30;
	}
	return(j);
}

/**
 * Decode the given string that was encoded using util_encode_bytes into the
 * binary data it represents and store it at the given data address up to the
 * given maximum number of bytes.  If the data cannot fit within the given
 * maximum then -1 will be returned.  Otherwise the given number of characters
 * in the string read is returned.
 */
int util_decode_bytes(const char *str, unsigned char *data, int max)
{
	int i, j;

	for (i = 0, j = 0;(str[i] != '\0') && (j < max);i += 2, j++)
		data[j] = ((str[i] - 0x30) << 4) + (str[i + 1] - 0x30);
	return(i);
}

/**
 * Convert the charcter escape sequence (assuming str starts after the escape
 * character) and stores the character in buffer[0].  The number of characters
 * read as a sequence from str is returned
 */
int util_escape_char(const char *str, char *buffer)
{
	char number[3];

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
			if ((str[1] != '\0') && (str[2] != '\0')) {
				number[0] = str[1];
				number[1] = str[2];
				number[2] = '\0';
				buffer[0] = util_atoi(number, 16);
			}
			return(3);
		default:
			if (is_number_char_m(str[0])) {
				buffer[0] = str[0] - 0x30;
				if (is_number_char_m(str[1])) {
					buffer[0] = (buffer[0] * 8) + str[1] - 0x30;
					if (is_number_char_m(str[2])) {
						buffer[0] = (buffer[0] * 8) + str[2] - 0x30;
						return(3);
					}
					return(2);
				}
				return(1);
			}
			else
				buffer[0] = str[0];
			break;
	}
	return(1);
}

/**
 * Convert the given key sequence representation into the sequence of
 * actual characters expected from the terminal.
 */
int util_convert_key(const unsigned char *str, int *key, int max)
{
	int i, j = 0, k;
	unsigned char buffer[SMALL_STRING_SIZE];

	max--;
	for (i = 0;(str[i] != '\0') && (j < max);) {
		if (str[i] == '$') {
			k = util_expand_variable(&str[i], buffer, SMALL_STRING_SIZE, &i);
			j += util_convert_key(buffer, &key[j], k);
			i++;
		}
		else if (str[i] == '\\') {
			i += util_escape_char(&str[i + 1], buffer) + 1;
			key[j++] = buffer[0];
		}
		else if (!strncmp_icase(&str[i], "u+", 2)) {
			i += 2;
			for (k = 0;(k < SMALL_STRING_SIZE) && (str[i] != '\0');k++, i++) {
				if (((str[i] >= 0x30) && (str[i] <= 0x39)) || ((str[i] >= 0x41) && (str[i] <= 0x46)) || ((str[i] >= 0x61) && (str[i] <= 0x66)))
					buffer[k] = str[i];
				else
					break;
			}
			buffer[k] = '\0';
			key[j++] = util_atoi(buffer, 16);
		}
		else if (str[i] == '^') {
			i++;
			if ((str[i] >= 0x41) && (str[i] <= 0x5f))
				key[j++] = str[i++] - 0x40;
			else if ((str[i] >= 0x61) && (str[i] <= 0x7a))
				key[j++] = str[i++] - 0x60;
		}
		else
			key[j++] = str[i++];
	}
	key[j] = '\0';
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
int util_expand_str(const char *fmt, char *buffer, int max)
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
int util_expand_variable(const char *str, char *buffer, int max, int *str_count)
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
int util_emit_str(void *obj, char *name, char *fmt, ...)
{
	va_list va;
	char buffer[STRING_SIZE];

	va_start(va, fmt);
	if (vsnprintf(buffer, STRING_SIZE, fmt, va) < 0)
		return(-1);
	return(emit_signal(obj, name, buffer));
}

/**
 * Execute the given command (without the command prefix) by looking up the
 * first space-delimited word as a variable and calling the evalute function
 * of that variable passing it the rest of the command string.  If an error
 * occurs then -1 is returned otherwise 0 is returned.
 */
int util_evaluate_command(char *cmd, char *str)
{
	void *value;
	struct type_s *type;

	if (!(value = index_variable(NULL, PATH_VARIABLE_NAME, cmd, &type)) && !(value = find_variable(NULL, cmd, &type)))
		return(-1);
	if (!type || !type->evaluate)
		return(-1);
	type->evaluate(value, str);
	return(0);
}

