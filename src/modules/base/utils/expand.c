/*
 * Module Name:		expand.c
 * Version:		0.1
 * Module Requirements:	type ; variable ; string
 * Description:		String Interpolator
 */

#include <stdlib.h>

#include <type.h>
#include <macros.h>
#include <variable.h>
#include <nit/string.h>

#define MAX_BUFFER		1024

static int copy_string(char *, char *, int);

/**
 * Allocate a string and expand any variable references in the given format
 * string.  If successful, a pointer to the string_t is returned; otherwise
 * NULL is returned
 */
string_t base_expand_str(char *str)
{
	int j;
	char buffer[MAX_BUFFER];

	j = copy_string(str, buffer, 0);
	buffer[j++] = '\0';

	return(create_string(buffer));
}

/*** Local Functions ***/

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
static int copy_string(char *str, char *buffer, int j)
{
	int i, k;
	char delim;
	int recurse;
	string_t value;
	char *name, *ns;
	struct variable_s *var;

	for (i = 0;(str[i] != '\0') && (j < MAX_BUFFER - 1);i++) {
		if (str[i] == '$') {
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
			for (k = j;(str[i] != '\0') && (str[i] != delim) && (k < MAX_BUFFER - 1);k++, i++)
				buffer[k] = str[i];
			buffer[k] = '\0';
			get_prefix_m(name, ns, ':');
			if ((var = find_variable(NULL, ns, name)) && var->type->stringify && (value = var->type->stringify(var->value))) {
				if (recurse)
					j += copy_string(value, buffer, j);
				else {
					strncpy(&buffer[j], value, (MAX_BUFFER - j - 1));
					j += strlen(value);	
				}
				destroy_string(value);
			}
			if (delim != '}')
				i--;
		}
		else
			buffer[j++] = str[i];
	}
	return(j);
}


