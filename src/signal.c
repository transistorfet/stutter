/*
 * Module Name:		signal.c
 * Description:		Signal Manager
 */

#include <string.h>

#include CONFIG_H
#include <stutter/signal.h>
#include <stutter/memory.h>
#include <stutter/macros.h>
#include <stutter/globals.h>
#include <stutter/variable.h>

static int signal_initialized = 0;
struct variable_table_s *signal_table = NULL;

static int signal_purge_handlers(struct signal_s *, struct signal_handler_s *);

struct variable_type_s signal_type = { {
	OBJECT_TYPE_S(&variable_table_type),
	"signal",
	sizeof(struct signal_s),
	NULL,
	(object_init_t) signal_init,
	(object_release_t) signal_release },
	(variable_add_t) add_variable_real,
	(variable_remove_t) remove_variable,
	(variable_index_t) find_variable,
	(variable_traverse_t) traverse_variable_table,
	(variable_stringify_t) NULL,
	(variable_evaluate_t) NULL
};

int init_signal(void)
{
	if (signal_initialized)
		return(1);
	if (!(signal_table = VARIABLE_TABLE_S(add_variable(NULL, OBJECT_TYPE_S(&variable_table_type), "signals", 0, ""))))
		return(-1);
	object_register_type(OBJECT_TYPE_S(&signal_type));
	signal_initialized = 1;
	return(0);
}

int release_signal(void)
{
	if (!signal_initialized)
		return(1);
	remove_variable(NULL, NULL, "signals");
	object_deregister_type(OBJECT_TYPE_S(&signal_type));
	signal_initialized = 0;
	return(0);
}

int signal_init(struct signal_s *signal, const char *params, va_list va)
{
	if (variable_table_init(VARIABLE_TABLE_S(signal), params, va))
		return(-1);
	return(0);
}

void signal_release(struct signal_s *signal)
{
	struct signal_handler_s *cur, *tmp;

	for (cur = signal->handlers; cur; cur = tmp) {
		tmp = cur->next;
		// TODO do we need to destroy the ptr we were given?
		memory_free(cur);
	}
	variable_table_release(VARIABLE_TABLE_S(signal));
}


/**
 * Connect a new handler to the signal with the given name so that the given
 * when that signal is emitted, the given function is called with the given
 * pointer passed as the first argument.  The given function must not be NULL
 * and the signal must already be created using the add_signal function.  If
 * an error occurs, -1 is returned, otherwise 0 is returned. 
 */
struct signal_handler_s *signal_connect(struct signal_s *signal, int priority, signal_t func, void *ptr)
{
	struct signal_handler_s *handler, *cur, *prev;

	// TODO if you want to allow for inline signal lookup like signal_connect(find_variable(...), ...)
	//	then you must check for a valid signal pointer
	if (!func)
		return(NULL);
	if (!(handler = memory_alloc(sizeof(struct signal_handler_s))))
		return(NULL);
	handler->bitflags = 0;
	handler->priority = priority;
	handler->func = func;
	handler->ptr = ptr;
	handler->parent = signal;
	handler->next = NULL;

	if (!signal->handlers)
		signal->handlers = handler;
	else {
		for (cur = signal->handlers, prev = NULL; cur; prev = cur, cur = cur->next) {
			if (priority >= cur->priority) {
				if (prev) {
					prev->next = handler;
					handler->next = cur;
				}
				else {
					handler->next = signal->handlers;
					signal->handlers = handler;
				}
				return(handler);
			}
		}
		/** The handler wasn't added so it should be added to the end */
		prev->next = handler;
		handler->next = NULL;
	}
	return(handler);
}

/**
 * Disconnects the given signal handler.
 */
int signal_disconnect(struct signal_handler_s *handler)
{
	return(signal_purge_handlers(handler->parent, handler));
}

/**
 * All of the handlers associated with the signal specified by the given name
 * are called with the object passed as the second argument and the given
 * pointer passed as the third argument.  The number of handlers called is
 * returned or -1 is returned if the signal is not found.
 */
int signal_emit(struct signal_s *signal, void *ptr)
{
	int ret;
	int calls = 0, purge = 0;
	struct signal_handler_s *cur;

	for (cur = signal->handlers; cur; cur = cur->next) {
		calls++;
		cur->bitflags |= SIG_BF_NODE_LOCKED;
		ret = cur->func(cur->ptr, ptr);
		cur->bitflags &= ~SIG_BF_NODE_LOCKED;
		if (cur->bitflags & SIG_BF_PURGE)
			purge = 1;
		if (ret == SIGNAL_STOP_EMIT)
			break;
	}
	if (purge)
		signal_purge_handlers(signal, NULL);
	return(calls);
}

/**
 * Return the pointer to the handler from the list of handlers for the given
 * object and signal name that matches the given function (or if NULL is given
 * then any function) and that match the given pointer (or if NULL is given
 * then any pointer).
 */
struct signal_handler_s *signal_find_handler(struct signal_s *signal, signal_t func, void *ptr)
{
	struct signal_handler_s *cur;

	for (cur = signal->handlers; cur; cur = cur->next) {
		if ((!func || (func == cur->func)) && (!ptr || (ptr == cur->ptr)))
			return(cur);
	}
	return(NULL);
}

/*** Local Functions ***/

/**
 * Remove all handlers from the given node that have the purge flag set and the
 * given handler if found.
 */
static int signal_purge_handlers(struct signal_s *signal, struct signal_handler_s *handler)
{
	int disconnected = 0;
	struct signal_handler_s *cur, *prev, *tmp;

	for (cur = signal->handlers, prev = NULL; cur; prev = cur, cur = cur->next) {
		if ((cur->bitflags & SIG_BF_PURGE) || (cur == handler)) {
			disconnected++;
			if (cur->bitflags & SIG_BF_NODE_LOCKED) {
				cur->bitflags |= SIG_BF_PURGE;
				prev = cur;
				cur = cur->next;
			}
			else {
				if (prev)
					prev->next = tmp = cur->next;
				else
					signal->handlers = tmp = cur->next;
				memory_free(cur);
				cur = tmp;
			}
		}
		else {
			prev = cur;
			cur = cur->next;
		}
	}
	return(disconnected);
}


