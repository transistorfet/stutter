/*
 * Header Name:	config-def.h
 * Description:	Default Configuration Header
 */

#ifndef _CONFIG_DEF_H
#define _CONFIG_DEF_H

#include <stutter/macros.h>
#include <stutter/config-macros.h>

#define DEFAULT_STUTTER_VERSION			"0.3.2"

#define DEFAULT_PATH_VARIABLE_NAME		"path"
#define DEFAULT_NAME_SEPARATOR			'.'

#define DEFAULT_LARGE_STRING_SIZE		1024
#define DEFAULT_STRING_SIZE			512
#define DEFAULT_SMALL_STRING_SIZE		128
#define DEFAULT_NAME_STRING_SIZE		64

#define DEFAULT_KEY_CONTEXT_LIST_INIT_SIZE	10
#define DEFAULT_KEY_SUBMAP_INIT_SIZE		20
#define DEFAULT_KEY_ROOTMAP_INIT_SIZE		20
#define DEFAULT_KEY_MAP_LOAD_FACTOR		0.75
#define DEFAULT_KEY_MAP_GROWTH_FACTOR		1.75

#define DEFAULT_VARIABLE_LIST_INIT_SIZE		20
#define DEFAULT_VARIABLE_LIST_LOAD_FACTOR	0.75
#define DEFAULT_VARIABLE_LIST_GROWTH_FACTOR	1.75

#define DEFAULT_SIGNAL_LIST_INIT_SIZE		20
#define DEFAULT_SIGNAL_LIST_LOAD_FACTOR		0.75
#define DEFAULT_SIGNAL_LIST_GROWTH_FACTOR	1.75

#define DEFAULT_TYPE_LIST_INIT_SIZE		20
#define DEFAULT_TYPE_LIST_LOAD_FACTOR		0.75
#define DEFAULT_TYPE_LIST_GROWTH_FACTOR		1.75

#ifdef WIN32
#define DEFAULT_DOT_FILE			"stutterrc"
#else
#define DEFAULT_DOT_FILE			"~/.stutterrc"
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
	PRINT_TO_CURRENT(__VA_ARGS__)

#define DEFAULT_OUTPUT_JOINPOINT(name, ...)	\
	PRINT_TO_CURRENT(__VA_ARGS__)

/*** Global Initialization ***/

#define DEFAULT_STUTTER_INIT_COMMANDS()				\
	EVALUATE_COMMAND("base.alias setcolour set fe.theme.")	\
	EVALUATE_COMMAND("base.alias getcolour get fe.theme.")	\

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
#ifndef KEY_CONTEXT_LIST_INIT_SIZE
#define KEY_CONTEXT_LIST_INIT_SIZE \
	DEFAULT_KEY_CONTEXT_LIST_INIT_SIZE
#endif
#ifndef KEY_SUBMAP_INIT_SIZE
#define KEY_SUBMAP_INIT_SIZE \
	DEFAULT_KEY_SUBMAP_INIT_SIZE
#endif
#ifndef KEY_ROOTMAP_INIT_SIZE
#define KEY_ROOTMAP_INIT_SIZE \
	DEFAULT_KEY_ROOTMAP_INIT_SIZE
#endif
#ifndef KEY_MAP_LOAD_FACTOR
#define KEY_MAP_LOAD_FACTOR \
	DEFAULT_KEY_MAP_LOAD_FACTOR
#endif
#ifndef KEY_MAP_GROWTH_FACTOR
#define KEY_MAP_GROWTH_FACTOR \
	DEFAULT_KEY_MAP_GROWTH_FACTOR
#endif
#ifndef VARIABLE_LIST_INIT_SIZE
#define VARIABLE_LIST_INIT_SIZE \
	DEFAULT_VARIABLE_LIST_INIT_SIZE
#endif
#ifndef VARIABLE_LIST_LOAD_FACTOR
#define VARIABLE_LIST_LOAD_FACTOR \
	DEFAULT_VARIABLE_LIST_LOAD_FACTOR
#endif
#ifndef VARIABLE_LIST_GROWTH_FACTOR
#define VARIABLE_LIST_GROWTH_FACTOR \
	DEFAULT_VARIABLE_LIST_GROWTH_FACTOR
#endif
#ifndef SIGNAL_LIST_INIT_SIZE
#define SIGNAL_LIST_INIT_SIZE \
	DEFAULT_SIGNAL_LIST_INIT_SIZE
#endif
#ifndef SIGNAL_LIST_LOAD_FACTOR
#define SIGNAL_LIST_LOAD_FACTOR \
	DEFAULT_SIGNAL_LIST_LOAD_FACTOR
#endif
#ifndef SIGNAL_LIST_GROWTH_FACTOR
#define SIGNAL_LIST_GROWTH_FACTOR \
	DEFAULT_SIGNAL_LIST_GROWTH_FACTOR
#endif
#ifndef TYPE_LIST_INIT_SIZE
#define TYPE_LIST_INIT_SIZE \
	DEFAULT_TYPE_LIST_INIT_SIZE
#endif
#ifndef TYPE_LIST_LOAD_FACTOR
#define TYPE_LIST_LOAD_FACTOR \
	DEFAULT_TYPE_LIST_LOAD_FACTOR
#endif
#ifndef TYPE_LIST_GROWTH_FACTOR
#define TYPE_LIST_GROWTH_FACTOR \
	DEFAULT_TYPE_LIST_GROWTH_FACTOR
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
#ifndef OUTPUT_JOINPOINT
#define OUTPUT_JOINPOINT(name, ...) \
	DEFAULT_OUTPUT_JOINPOINT(name, __VA_ARGS__)
#endif
#ifndef STUTTER_INIT_COMMANDS
#define STUTTER_INIT_COMMANDS() \
	DEFAULT_STUTTER_INIT_COMMANDS()
#endif
#ifndef STUTTER_INIT
#define STUTTER_INIT(argc, argv) \
	DEFAULT_STUTTER_INIT(argc, argv)
#endif
/*** END OF DEFAULT ASSIGNMENTS ***/


#endif


