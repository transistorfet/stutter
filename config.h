/*
 * Header Name:	config.h
 * Description:	Configuration Header (Default)
 */

#ifndef _CONFIG_H
#define _CONFIG_H

#include <stutter/config-macros.h>

#define BASE_DEFAULT_PATH		";irc;base"

#define MODULE_LIST() 		\
	MODULE(base)		\
	MODULE(irc)

#include <stutter/config-def.h>
#include <stutter/frontend/config-def.h>
#include <stutter/modules/base/config-def.h>
#include <stutter/modules/irc/config-def.h>

#endif


