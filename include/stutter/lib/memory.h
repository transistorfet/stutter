/*
 * Header Name:		memory.h
 * Version:		0.1
 * Description:		Memory Allocator and Tracker Header
 */

#ifndef _STUTTER_LIB_MEMORY_H
#define _STUTTER_LIB_MEMORY_H

#include <stdlib.h>

void *memory_alloc(unsigned int);
void *memory_realloc(void *, unsigned int);
void memory_free(void *);

#endif

