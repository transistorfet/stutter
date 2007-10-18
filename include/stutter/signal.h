/*
 * Header Name:		signal.h
 * Description:		Signal Manager Header
 */

#ifndef _STUTTER_SIGNAL_H
#define _STUTTER_SIGNAL_H

#include <stutter/string.h>
#include <stutter/globals.h>

#define SIGNAL_STOP_EMIT		1

#define SIG_HBF_NODE_LOCKED		0x0001
#define SIG_HBF_PURGE			0x0002

typedef int (*signal_t)(void *, void *, void *);

struct signal_s;

struct signal_handler_s {
	int bitflags;
	int priority;
	signal_t func;
	void *ptr;
	struct signal_s *parent;
	struct signal_handler_s *next;
};

int init_signal(void);
int release_signal(void);

int add_signal(void *, char *, int);
int remove_signal(void *, char *);
int emit_signal(void *, char *, void *);
struct signal_handler_s *signal_connect(void *, char *, int, signal_t, void *);
int signal_disconnect(struct signal_handler_s *);
struct signal_handler_s *signal_find_handler(void *, char *, signal_t, void *);

#endif


