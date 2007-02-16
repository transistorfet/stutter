/*
 * Header Name:		globals.h
 * Version:		0.1
 * Description:		Global Definitions
 */

#ifndef _STUTTER_GLOBALS_H
#define _STUTTER_GLOBALS_H

#define ERR_OUT_OF_MEMORY		-10
#define ERR_COMMAND_NOT_FOUND		-11

#define IO_COND_READ			0x01
#define IO_COND_WRITE			0x02
#define IO_COND_ERROR			0x04

typedef void (*destroy_t)(void *);
typedef int (*compare_t)(void *, void *);
typedef int (*traverse_t)(void *, void *);
typedef int (*callback_t)(void *, void *);

struct callback_s {
	callback_t func;
	void *ptr;
};

#define execute_callback_m(callback, arg)	\
	if ((callback).func)			\
		((callback).func)((callback).ptr, arg)

#endif


