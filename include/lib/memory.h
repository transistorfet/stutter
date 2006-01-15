/*
 * Header Name:		memory.h
 * Version:		0.1
 * Description:		Memory Allocator and Tracker Header
 */

#ifndef _LIB_MEMORY_H
#define _LIB_MEMORY_H

#include <stdlib.h>

void *memory_alloc(unsigned int);
void memory_free(void *);

#endif

