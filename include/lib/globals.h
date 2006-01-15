/*
 * Header Name:		globals.h
 * Version:		0.1
 * Description:		Global Definitions
 */

#ifndef _LIB_GLOBALS_H
#define _LIB_GLOBALS_H

#define ERR_OUT_OF_MEMORY		-10
#define ERR_COMMAND_NOT_FOUND		-11

typedef void (*destroy_t)(void *);
typedef int (*compare_t)(void *, void *);
typedef int (*traverse_t)(void *, void *);
typedef int (*callback_t)(void *, void *);

#endif


