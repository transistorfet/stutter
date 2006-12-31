/*
 * Header Name:	config-def.h
 * Description:	Default Configuration Header
 */

#ifndef _CONFIG_DEF_H
#define _CONFIG_DEF_H

#include <stutter/config-macros.h>

#define DEFAULT_STUTTER_VERSION		"pre-0.3"

#define DEFAULT_PATH_VARIABLE_NAME	"path"
#define DEFAULT_NAME_SEPARATOR		'.'

#define DEFAULT_LARGE_STRING_SIZE	1024
#define DEFAULT_STRING_SIZE		512
#define DEFAULT_SMALL_STRING_SIZE	128
#define DEFAULT_NAME_STRING_SIZE	64

#ifdef WIN32
#define DEFAULT_DOT_FILE		"stutterrc"
#else
#define DEFAULT_DOT_FILE		"~/.stutterrc"
#endif

/*** Modules ***/

#define DEFAULT_MODULE_list()			\
	MODULE(base);

/*** Joinpoints ***/

#define DEFAULT_SYSTEM_INIT_JOINPOINT()		\
	DO_NOTHING()

#define DEFAULT_SYSTEM_RELEASE_JOINPOINT()	\
	DO_NOTHING()

#define DEFAULT_ERROR_JOINPOINT(name, ...)	\
	EMIT_STRING(name, NULL, __VA_ARGS__)

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
#ifndef LARGE_STRING_SIZE
#define LARGE_STRING_SIZE \
	DEFAULT_LARGE_STRING_SIZE
#endif
#ifndef STRING_SIZE
#define STRING_SIZE \
	DEFAULT_STRING_SIZE
#endif
#ifndef SMALL_STRING_SIZE
#define SMALL_STRING_SIZE \
	DEFAULT_SMALL_STRING_SIZE
#endif
#ifndef NAME_STRING_SIZE
#define NAME_STRING_SIZE \
	DEFAULT_NAME_STRING_SIZE
#endif
#ifndef DOT_FILE
#define DOT_FILE \
	DEFAULT_DOT_FILE
#endif
#ifndef DOT_FILE
#define DOT_FILE \
	DEFAULT_DOT_FILE
#endif
#ifndef MODULE_list
#define MODULE_list() \
	DEFAULT_MODULE_list()
#endif
#ifndef SYSTEM_INIT_JOINPOINT
#define SYSTEM_INIT_JOINPOINT() \
	DEFAULT_SYSTEM_INIT_JOINPOINT()
#endif
#ifndef SYSTEM_RELEASE_JOINPOINT
#define SYSTEM_RELEASE_JOINPOINT() \
	DEFAULT_SYSTEM_RELEASE_JOINPOINT()
#endif
#ifndef ERROR_JOINPOINT
#define ERROR_JOINPOINT(name, ...) \
	DEFAULT_ERROR_JOINPOINT(name, __VA_ARGS__)
#endif
#ifndef STUTTER_INIT
#define STUTTER_INIT(argc, argv) \
	DEFAULT_STUTTER_INIT(argc, argv)
#endif
/*** END OF DEFAULT ASSIGNMENTS ***/


#endif


