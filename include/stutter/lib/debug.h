/*
 * Header Name:		debug.h
 * Version:		0.1
 * Description:		Debug Macros Header
 */

#ifndef _NIT_DEBUG_H
#define _NIT_DEBUG_H

#include <stdio.h>

#ifdef DEBUG
	#define DEBUG_LOG(file, str) {			\
		FILE *debug_fptr;			\
		if (debug_fptr = fopen(file, "a")) {	\
			fprintf(debug_fptr, "%s", str);	\
			fclose(debug_fptr);		\
		}					\
	}
#else
	#define DEBUG_LOG(file, str)
#endif

#endif


