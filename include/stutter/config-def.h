/*
 * Header Name:	config-def.h
 * Description:	Default Base Configuration Header
 */

#ifndef _CONFIG_DEF_H
#define _CONFIG_DEF_H

#include <stutter/config-macros.h>

#define DEFAULT_STUTTER_VERSION		"pre-0.3"

/* Frontend Values */
#define FE_STATUSBAR			"$&fe.statusbar"
#define FE_STATUSBAR_DEFAULT		"${fe.inverse} $time $irc.current_nick on $irc.current_channel"
#define FE_STATUS_BAR_HEIGHT		1
#define FE_WINDOW_LOG_SIZE		1000
#define FE_WINDOW_WRAP_STRING		"        "

#define FE_BINDINGS()				\
	BIND_KEY("\x0b", "fe.insert", "\x03")

/*** General Values ***/

#define DEFAULT_PATH_VARIABLE_NAME	"path"
#define DEFAULT_NAME_SEPARATOR		'.'

#define DEFAULT_STRING_SIZE		512

#define DEFAULT_DOT_FILE		"~/.stutterrc"

/*** Modules ***/

#define DEFAULT_LOAD_MODULES()			\
	init_base();

#define DEFAULT_RELEASE_MODULES()		\
	release_base();

/*** Joinpoints ***/

#define DEFAULT_SYSTEM_INIT_JOINPOINT()		\
	DO_NOTHING();

#define DEFAULT_SYSTEM_RELEASE_JOINPOINT()	\
	DO_NOTHING();

/*** Global Initialization ***/

#define DEFAULT_STUTTER_INIT(argc, argv) {			\
	util_evaluate_command("base.source", DOT_FILE);		\
}

/*** START OF DEFAULT ASSIGNMENTS ***/
#ifndef STUTTER_VERSION
#define STUTTER_VERSION \
	DEFAULT_STUTTER_VERSION
#endif
#ifndef PATH_VARIABLE_NAME
#define PATH_VARIABLE_NAME \
	DEFAULT_PATH_VARIABLE_NAME
#endif
#ifndef NAME_SEPARATOR
#define NAME_SEPARATOR \
	DEFAULT_NAME_SEPARATOR
#endif
#ifndef STRING_SIZE
#define STRING_SIZE \
	DEFAULT_STRING_SIZE
#endif
#ifndef DOT_FILE
#define DOT_FILE \
	DEFAULT_DOT_FILE
#endif
#ifndef LOAD_MODULES
#define LOAD_MODULES() \
	DEFAULT_LOAD_MODULES()
#endif
#ifndef RELEASE_MODULES
#define RELEASE_MODULES() \
	DEFAULT_RELEASE_MODULES()
#endif
#ifndef SYSTEM_INIT_JOINPOINT
#define SYSTEM_INIT_JOINPOINT() \
	DEFAULT_SYSTEM_INIT_JOINPOINT()
#endif
#ifndef SYSTEM_RELEASE_JOINPOINT
#define SYSTEM_RELEASE_JOINPOINT() \
	DEFAULT_SYSTEM_RELEASE_JOINPOINT()
#endif
#ifndef STUTTER_INIT
#define STUTTER_INIT(argc, argv) \
	DEFAULT_STUTTER_INIT(argc, argv)
#endif
/*** END OF DEFAULT ASSIGNMENTS ***/


#endif


