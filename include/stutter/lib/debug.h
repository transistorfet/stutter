/*
 * Header Name:		debug.h
 * Version:		0.1
 * Description:		Debug Macros Header
 */

#ifndef _STUTTER_LIB_DEBUG_H
#define _STUTTER_LIB_DEBUG_H

#include <stdio.h>

#ifdef DEBUG
	#define DEBUG_LOG(file, ...) {				\
		FILE *debug_fptr;				\
		if (debug_fptr = fopen(file, "a")) {		\
			fprintf(debug_fptr, __VA_ARGS__);	\
			fclose(debug_fptr);			\
		}						\
	}
#else
	#define DEBUG_LOG(file, str)
#endif

#ifdef DEBUG
	#define DEBUG_LOG_LIMITED(file, str, num) {		\
		FILE *debug_fptr;				\
		if (debug_fptr = fopen(file, "a")) {		\
			fwrite(str, num, 1, debug_fptr);	\
			fclose(debug_fptr);			\
		}						\
	}
#else
	#define DEBUG_LOG_LIMITED(file, str, num)
#endif

#endif


