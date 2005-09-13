/*
 * Header Name:		command.h
 * Version:		0.1
 * Description:		Command Manager Header
 */

#ifndef _COMMAND_H
#define _COMMAND_H

#include <nit/callback.h>

int init_command(void);
int release_command(void);

int add_command(char *, callback_t, void *);
int remove_command(char *);
int evaluate_command(char *, void *);

#endif


