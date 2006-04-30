/*
 * Module Name:		memory.c
 * Version:		0.1
 * Module Requirements:	(none)
 * Description:		Memory Allocator and Tracker
 */

#include <stdlib.h>

#include <stutter/lib/memory.h>

/**
 * Allocate a block of memory of the given size and return a pointer to it.
 */
void *memory_alloc(unsigned int size)
{
	return(malloc(size));
}

/**
 * Reallocate the given block of memory to the given size and return a pointer to it.
 */
void *memory_realloc(void *addr, unsigned int size)
{
	return(realloc(addr, size));
}

/**
 * Free the memory pointed to by the given pointer.
 */
void memory_free(void *addr)
{
	free(addr);
}


