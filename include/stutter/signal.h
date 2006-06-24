/*
 * Header Name:		signal.h
 * Version:		0.1
 * Description:		Signal Manager Header
 */

#ifndef _STUTTER_SIGNAL_H
#define _STUTTER_SIGNAL_H

#include <stutter/lib/string.h>
#include <stutter/lib/globals.h>

typedef int (*signal_t)(void *, void *);

struct signal_handler_s {
	void *index;
	signal_t func;
	void *ptr;
	struct signal_handler_s *next;
};

struct signal_s {
	char *name;
	int bitflags;
	struct signal_handler_s *handlers;
};

/*
int remove_signal_by_name(char *, void *);
int signal_connect_by_name(char *, void *, signal_t, void *);
int signal_disconnect_by_name(char *, void *, signal_t, void *);
int signal_emit_by_name(char *, void *, void *);
*/

int init_signal(void);
int release_signal(void);

/*
struct signal_s *add_signal(char *, int);
int remove_signal(struct signal_s *);
struct signal_s *find_signal(char *, void *);
int signal_connect(struct signal_s *, signal_t, void *);
int signal_disconnect(struct signal_s *, signal_t, void *);
int signal_emit(struct signal_s *, void *);
*/

int add_signal(char *, int);
int remove_signal(char *);
int signal_connect(char *, void *, signal_t, void *);
int signal_disconnect(char *, void *, signal_t, void *);
int signal_emit(char *, void *, void *);

/*
int add_signal(char *);
int remove_signal(char *);
int signal_connect(char *, signal_t, void *);
int signal_disconnect(char *, signal_t, void *);
int signal_emit(char *, void *);
*/

#endif


