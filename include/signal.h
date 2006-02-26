/*
 * Header Name:		signal.h
 * Version:		0.1
 * Description:		Signal Manager Header
 */

#ifndef _SIGNAL_H
#define _SIGNAL_H

#include <lib/string.h>
#include <lib/globals.h>

typedef int (*signal_t)(void *, void *);

int init_signal(void);
int release_signal(void);

int add_signal(char *);
int remove_signal(char *);
int signal_connect(char *, signal_t, void *);
int signal_disconnect(char *, signal_t, void *);
int signal_emit(char *, void *);

#endif


