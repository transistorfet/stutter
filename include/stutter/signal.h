/*
 * Header Name:		signal.h
 * Description:		Signal Manager Header
 */

#ifndef _STUTTER_SIGNAL_H
#define _STUTTER_SIGNAL_H

#include <stutter/string.h>
#include <stutter/globals.h>
#include <stutter/variable.h>

#define SIGNAL_STOP_EMIT		1

#define SIG_BF_NODE_LOCKED		0x0001
#define SIG_BF_PURGE			0x0002

#define SIGNAL_S(ptr)			( (struct signal_s *) (ptr) )

// TODO what should the actual parameters be to this function?
typedef int (*signal_t)(void *, void *);

struct signal_handler_s {
	int bitflags;
	int priority;
	signal_t func;
	// TODO what will this actually be?  will we assist in managing it (destroy if an object, etc)?
	void *ptr;
	struct signal_s *parent;
	struct signal_handler_s *next;
};

struct signal_s {
	struct variable_table_s variable_table;
	struct signal_handler_s *handlers;
};

extern struct variable_type_s signal_type;
extern struct variable_table_s *signal_table;

int init_signal(void);
int release_signal(void);

int signal_init(struct signal_s *, const char *, va_list);
void signal_release(struct signal_s *);

struct signal_handler_s *signal_connect(struct signal_s *, int, signal_t, void *);
int signal_disconnect(struct signal_handler_s *);
int signal_emit(struct signal_s *, void *);
struct signal_handler_s *signal_find_handler(struct signal_s *, signal_t, void *);

static inline struct signal_s *add_signal(struct variable_table_s *table, const char *name) {
	return(SIGNAL_S(add_variable(table, OBJECT_TYPE_S(&signal_type), name, 0, "")));
}

static inline struct signal_handler_s *signal_named_connect(struct variable_table_s *table, const char *name, int priority, signal_t func, void *ptr) {
	struct signal_s *signal;

	if (!(signal = SIGNAL_S(find_variable(table, name, OBJECT_TYPE_S(&signal_type)))))
		return(NULL);
	return(signal_connect(signal, priority, func, ptr));
}

static inline int signal_named_emit(struct variable_table_s *table, const char *name, void *arg) {
	struct signal_s *signal;

	if (!(signal = SIGNAL_S(find_variable(table, name, OBJECT_TYPE_S(&signal_type)))))
		return(0);
	return(signal_emit(signal, arg));
}

#endif


