/*
 * Header Name:		macros.h
 * Version:		0.1
 * Description:		Macro Definitions
 */

#ifndef _MACROS_H
#define _MACROS_H

#include <string.h>

#define get_param_m(str, cmd, ch)	\
	cmd = str;			\
	if (str = strchr(str, ch)) {	\
		*str = '\0';		\
		str = &str[1];		\
	}				\
	else				\
		str = "";

#define get_prefix_m(str, prefix, ch)	\
	prefix = str;			\
	if (str = strchr(str, ch)) {	\
		*str = '\0';		\
		str = &str[1];		\
	}				\
	else {				\
		str = prefix;		\
		prefix = NULL;		\
	}

#endif


