/*
 * Module Name:		execute.c
 * System Requirements:	unistd.h
 * Description:		Program Execution Manager
 */


#include <string.h>

#include <stutter/memory.h>
#include <stutter/globals.h>

typedef void *fe_execute_t;

int init_execute(void)
{
	return(0);
}

int release_execute(void)
{
	return(0);
}

/**
 * Execute the given command and return a reference to the running program.
 */
fe_execute_t fe_execute_open(char *cmd, int bitflags)
{
	// TODO dummy. fill in later
	return(NULL);
}

/**
 * Close the given program reference.
 */
void fe_execute_close(fe_execute_t desc)
{
	// TODO nothing here
}

/**
 * Returns the callback for the given process.
 */
struct callback_s fe_execute_get_callback(fe_execute_t desc)
{
	// TODO this isn't really here ;)
	struct callback_s callback = { 0 };
	return(callback);
}

/**
 * Sets the callback for the given process to be executed under the given
 * conditions.
 */
void fe_execute_set_callback(fe_execute_t desc, int condition, callback_t func, void *ptr)
{
	// TODO nothing here
}

/**
 * Send the string of length len to the given process and
 * return the number of bytes written or -1 on error.
 */
int fe_execute_send(fe_execute_t desc, char *buffer, int len)
{
	return(-1);
}

/**
 * Receive the given number of bytes, store them in the given buffer
 * and return the number of bytes read or -1 on error or disconnect.
 */ 
int fe_execute_receive(fe_execute_t desc, char *buffer, int len)
{
	return(-1);
}

/**
 * Receive a string from the descriptor up to a maximum of
 * len-1 (a null char is appended) and return the number of bytes
 * read or -1 on error or disconnect.
 */ 
int fe_execute_receive_str(fe_execute_t desc, char *buffer, int len, char ch)
{
	return(-1);
}

