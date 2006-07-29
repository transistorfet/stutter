/*
 * Header Name:		signal.h
 * Version:		0.1
 * Description:		Signal Manager Header
 */

#ifndef _STUTTER_SIGNAL_H
#define _STUTTER_SIGNAL_H

#include <stutter/lib/string.h>
#include <stutter/lib/globals.h>

#define SIGNAL_STOP_EMIT		1

#define SIG_BF_USE_WILDCARD_INDEX	0x0001

typedef int (*signal_t)(void *, void *);

struct signal_handler_s {
	void *index;
	int priority;
	signal_t func;
	void *ptr;
	struct signal_handler_s *next;
};

struct signal_s {
	char *name;
	int bitflags;
	struct signal_handler_s *handlers;
};

int init_signal(void);
int release_signal(void);

int add_signal(char *, int);
int remove_signal(char *);
int signal_connect(char *, void *, int, signal_t, void *);
int signal_disconnect(char *, void *, signal_t, void *);
int signal_emit(char *, void *, void *);

#endif


