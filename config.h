/*
 * Header Name:	config.h
 * Description:	Configuration Header (Default)
 */

#ifndef _CONFIG_H
#define _CONFIG_H

#include <stutter/config-macros.h>

#define BASE_DEFAULT_PATH		";irc;base"
#define FE_STATUSBAR_DEFAULT		"${fe.theme.statusbar} $time $irc.current_nick on $irc.current_channel"

#define MODULE_LIST() 		\
	MODULE(base)		\
	MODULE(irc)

#define ERROR_JOINPOINT(name, ...)	\
	PRINT_TO_CURRENT(__VA_ARGS__)

#include <stutter/config-def.h>
#include <stutter/frontend/config-def.h>
#include <stutter/modules/base/config-def.h>
#include <stutter/modules/irc/config-def.h>

#endif


