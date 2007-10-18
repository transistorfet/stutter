/*
 * Header Name:		memory.h
 * Description:		Memory Allocator and Tracker Header
 */

#ifndef _STUTTER_MEMORY_H
#define _STUTTER_MEMORY_H

#include <stdlib.h>

void *memory_alloc(unsigned int);
void *memory_realloc(void *, unsigned int);
void memory_free(void *);

#endif

