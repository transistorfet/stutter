/*
 * Header Name:	config.h
 * Description:	Configuration Header (Default)
 */

#ifndef _CONFIG_H
#define _CONFIG_H

#include <stutter/config-macros.h>

#define BASE_DEFAULT_PATH		";irc;base"

#define LOAD_MODULES()			\
	init_base();			\
	init_irc();

#define RELEASE_MODULES()		\
	release_irc();			\
	release_base();

#include <stutter/config-def.h>
#include <stutter/modules/base/config-def.h>
#include <stutter/modules/irc/config-def.h>

#endif


