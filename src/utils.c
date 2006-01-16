/*
 * Module Name:		utils.c
 * Version:		0.1
 * Module Requirements:	type ; string ; variable
 * Description:		Utilities
 */

#include <stdlib.h>

#include <type.h>
#include <variable.h>
#include <lib/macros.h>
#include <lib/string.h>
#include <utils.h>

#define MAX_NAME		64
#define MAX_BUFFER		1024

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
string_t util_convert_key(char *str)
{
	int i, j = 0;
	char buffer[MAX_BUFFER];

	for (i = 0;str[i] != '\0';i++) {
		if (j >= MAX_BUFFER)
			break;
		if (str[i] == '$')
			j += util_expand_variable_to_buffer(&str[i], &buffer[j], MAX_BUFFER - j, &i);
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
	return(create_string("%s", buffer));
}

/**
 * Allocate a string and expand any variable references in the given format
 * string.  If successful, a pointer to the string_t is returned; otherwise
 * NULL is returned
 */
string_t util_expand_str(char *str)
{
	int j;
	char buffer[MAX_BUFFER];

	j = util_expand_to_buffer(str, buffer, MAX_BUFFER);
	buffer[j++] = '\0';

	return(create_string(buffer));
}

/**
 * Copy the given str into the given buffer using the given j as the
 * start of the buffer.  The index into the buffer of the last
 * written character plus 1 is returned.  If a $ is encountered, the
 * substring up to the next space is taken to be a variable name.  If
 * the name is enclosed in { }, then the variable name is take to be
 * up to the closing }.  The variable is found and the stringify
 * function is called on it.  The resulting string is copied into the
 * buffer.  If a & follows the $ then the stringified string is
 * recursed on expanding any variable references in it.
 */
int util_expand_to_buffer(char *str, char *buffer, int max)
{
	int i, k;
	int j = 0;
	char delim;
	int recurse;
	string_t value;
	char *name, *ns;
	struct variable_s *var;

	for (i = 0;(str[i] != '\0') && (j < max - 1);i++) {
		if (str[i] == '\\') {
			if (str[++i] == '\0')
				break;
			i += util_escape_char(&str[i], &buffer[j++]) - 1;
		}
		else if (str[i] == '$') {
			j += util_expand_variable_to_buffer(&str[i], &buffer[j], max - j, &i);
/*
			if (str[++i] == '&') {
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
                
			name = &buffer[j];
			for (k = j;(str[i] != '\0') && (str[i] != delim) && (k < max - 1);k++, i++)
				buffer[k] = str[i];
			buffer[k] = '\0';
			get_prefix_m(name, ns, ':');
			if ((var = find_variable(NULL, ns, name)) && var->type->stringify && (value = var->type->stringify(var->value))) {
				if (recurse)
					j += util_expand_to_buffer(value, &buffer[j], max - j);
				else {
					strncpy(&buffer[j], value, (max - j - 1));
					j += strlen(value);	
				}
				destroy_string(value);
			}
			if (delim != '}')
				i--;
*/
		}
		else
			buffer[j++] = str[i];
	}
	return(j);
}

string_t util_expand_variable(char *str, int *count)
{
	int k;
	int i = 0;
	char delim;
	char *name, *ns;
	char buffer[MAX_NAME];
	string_t value = NULL;
	struct variable_s *var;

	if (str[i] == '$')
		i++;

	if (str[i] == '&')
		i++;

	if (str[i] == '{') {
		delim = '}';
		i++;
	}
	else
		delim = ' ';

	for (k = 0;(str[i] != '\0') && (str[i] != delim) && (k < MAX_NAME - 1);k++, i++)
		buffer[k] = str[i];
	buffer[k] = '\0';
	name = buffer;
	get_prefix_m(name, ns, ':');
	if ((var = find_variable(NULL, ns, name)) && var->type->stringify)
		value = var->type->stringify(var->value);
	if (delim != '}')
		i--;

	if (count)
		*count += i;
	return(value);

}

/**
 * Expand the variable referenced by the given str and copy the stringified
 * value to the given buffer up to the max number of characters.  The number
 * of characters written to the buffer is returned.  If the given str_count
 * pointer is not NULL then the number of characters used as the variable
 * name in str is added to the value it points to.  The given str may
 * start with the '$' char or may start just after it.
 */
int util_expand_variable_to_buffer(char *str, char *buffer, int max, int *str_count)
{
	int k;
	char delim;
	int recurse;
	string_t value;
	char *name, *ns;
	int i = 0, j = 0;
	struct variable_s *var;

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

	name = &buffer[j];
	for (k = j;(str[i] != '\0') && (str[i] != delim) && (k < max - 1);k++, i++)
		buffer[k] = str[i];
	buffer[k] = '\0';
	get_prefix_m(name, ns, ':');
	if ((var = find_variable(NULL, ns, name)) && var->type->stringify && (value = var->type->stringify(var->value))) {
		if (recurse)
			j += util_expand_to_buffer(value, &buffer[j], max - j);
		else {
			strncpy(&buffer[j], value, (max - j - 1));
			j += strlen(value);	
		}
		destroy_string(value);
	}
	if (delim != '}')
		i--;

	if (str_count)
		*str_count += i;
	return(j);
}

