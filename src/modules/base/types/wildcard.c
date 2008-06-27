/*
 * Type Name:		wildcard.c
 * Description:		Wildcard Type
 */

#include <string.h>
#include <stdarg.h>

#include <stutter/object.h>
#include <stutter/variable.h>
#include <stutter/modules/base.h>

struct variable_type_s base_wildcard_type = { {
	OBJECT_TYPE_S(&base_string_type),
	"wildcard",
	sizeof(struct base_string_s),
	NULL,
	(object_init_t) base_string_init,
	(object_release_t) base_string_release },
	(variable_add_t) NULL,
	(variable_remove_t) NULL,
	(variable_index_t) base_wildcard_index,
	(variable_traverse_t) NULL,
	(variable_stringify_t) base_string_stringify,
	(variable_evaluate_t) NULL
};

struct variable_s *base_wildcard_index(struct base_string_s *var, const char *name, struct object_type_s *type)
{
	int i;
	char ch, *str;
	struct variable_s *ret;

	for (i = 0, str = var->str; var->str[i++] != '\0'; i++) {
		if ((var->str[i] == ';') || (var->str[i] == '\0')) {
			ch = var->str[i];
			var->str[i] = '\0';
			if (((*str == '\0') && (ret = find_variable(NULL, name, type)))
			    || (ret = index_variable(NULL, str, name, type))) {
				var->str[i] = ch;
				return(ret);
			}
			if (ch != '\0') {
				var->str[i] = ch;
				str = &var->str[i + 1];
			}
		}
	}
	return(NULL);
}

