/*
 * Module Name:		init.c
 * Description:		Base Module Initializer
 */

#include <stdlib.h>

#include CONFIG_H
#include <stutter/init.h>
#include <stutter/type.h>
#include <stutter/signal.h>
#include <stutter/string.h>
#include <stutter/globals.h>
#include <stutter/variable.h>
#include <stutter/modules/base.h>

int init_base(void)
{
	add_signal(NULL, "base.error", 0);
	add_signal(NULL, "base.exec_output", 0);

	BASE_INIT_JOINPOINT()

	return(0);
}

int release_base(void)
{
	struct type_s *type;

	BASE_RELEASE_JOINPOINT()

	/** Remove the whole variable table */
	if ((type = find_type("table"))) {
		add_variable(NULL, type, "base", 0, "");
		remove_variable(NULL, type, "base");
	}

	remove_signal(NULL, "base.exec_output");
	remove_signal(NULL, "base.error");
	return(0);
}


