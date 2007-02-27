/*
 * Module Name:		signal.c
 * Version:		0.1
 * Module Requirements:	memory ; hash
 * Description:		Signal Manager
 */


#include <string.h>

#include CONFIG_H
#include <stutter/hash.h>
#include <stutter/signal.h>
#include <stutter/memory.h>
#include <stutter/macros.h>
#include <stutter/globals.h>

#define signal_hash(list, key)			sdbm_hash_icase(key)
#define signal_compare_node(node, ptr)		(!strcmp_icase(node->name, ptr))

#define object_hash(list, key)			((unsigned int) key)
#define object_compare_node(node, ptr)		(node->obj == ptr)

struct signal_s {
	char *name;
	int bitflags;
	struct signal_handler_s *handlers;
	hash_node_v(signal_s) signals;
};

struct signal_list_s {
	void *obj;
	hash_list_v(signal_s) signals;
	hash_node_v(signal_list_s) objects;
};

struct object_list_s {
	hash_list_v(signal_list_s) objects;
};

static int signal_initialized = 0;
static struct signal_list_s signal_list;
static struct object_list_s object_list;

static inline struct signal_s *create_signal(struct signal_list_s *, char *, int);
static inline void signal_release_node(struct signal_list_s *, struct signal_s *);
static inline struct signal_list_s *create_object(void *);
static inline void object_release_node(struct object_list_s *, struct signal_list_s *);
static int signal_purge_handlers(struct signal_s *, struct signal_handler_s *);

DEFINE_HASH_TABLE(signal, struct signal_list_s, struct signal_s, signals, name, signal_release_node, signal_hash, signal_compare_node, SIGNAL_LIST_LOAD_FACTOR, SIGNAL_LIST_GROWTH_FACTOR)
DEFINE_HASH_TABLE(object, struct object_list_s, struct signal_list_s, objects, obj, object_release_node, object_hash, object_compare_node, SIGNAL_OBJECTS_LOAD_FACTOR, SIGNAL_OBJECTS_GROWTH_FACTOR)

int init_signal(void)
{
	if (signal_initialized)
		return(1);
	object_init_table(&object_list, SIGNAL_OBJECTS_INIT_SIZE);
	signal_init_table(&signal_list, SIGNAL_LIST_INIT_SIZE);
	signal_initialized = 1;
	return(0);
}

int release_signal(void)
{
	if (!signal_initialized)
		return(1);
	signal_release_table(&signal_list);
	object_release_table(&object_list);
	signal_initialized = 0;
	return(0);
}

/**
 * Add a new signal with the given name to the list of signals.
 */
int add_signal(char *name, int bitflags)
{
	if (!create_signal(&signal_list, name, bitflags))
		return(-1);
	return(0);
}

/**
 * Remove the signal with the given name and destroy all handlers associated
 * with that signal.  If the signal cannot be found, -1 is returned,
 * otherwise 0 is returned.
 */
int remove_signal(char *name)
{
	signal_delete_node(&signal_list, name);
	return(0);
}

/**
 * All of the handlers associated with the signal specified by the given name
 * are called with the object passed as the second argument and the given
 * pointer passed as the third argument.  The number of handlers called is
 * returned or -1 is returned if the signal is not found.
 */
int emit_signal(void *obj, char *name, void *ptr)
{
	int ret;
	struct signal_s *signal;
	int calls = 0, purge = 0;
	struct signal_list_s *list;
	struct signal_handler_s *cur;

	if (!obj)
		list = &signal_list;
	else if (!(list = object_find_node(&object_list, obj)))
		return(-1);

	if (!(signal = signal_find_node(list, name)))
		return(-1);
	cur = signal->handlers;
	while (cur) {
		calls++;
		cur->bitflags |= SIG_HBF_NODE_LOCKED;
		ret = cur->func(cur->ptr, obj, ptr);
		cur->bitflags &= ~SIG_HBF_NODE_LOCKED;
		if (cur->bitflags & SIG_HBF_PURGE)
			purge = 1;
		if (ret == SIGNAL_STOP_EMIT)
			break;
		cur = cur->next;
	}
	if (purge)
		signal_purge_handlers(signal, NULL);
	return(calls);
}

/**
 * Connect a new handler to the signal with the given name so that the given
 * when that signal is emitted, the given function is called with the given
 * pointer passed as the first argument.  The given function must not be NULL
 * and the signal must already be created using the add_signal function.  If
 * an error occurs, -1 is returned, otherwise 0 is returned. 
 */
struct signal_handler_s *signal_connect(void *obj, char *name, int priority, signal_t func, void *ptr)
{
	struct signal_s *signal;
	struct signal_list_s *list;
	struct signal_handler_s *handler, *cur, *prev;

	if (!func)
		return(NULL);
	if (!obj)
		list = &signal_list;
	else if (!(list = object_find_node(&object_list, obj)) && !(list = create_object(obj)))
		return(NULL);

	if (!(signal = signal_find_node(list, name)) && !(signal = create_signal(list, name, 0)))
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
		prev = NULL;
		cur = signal->handlers;
		while (cur) {
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
			prev = cur;
			cur = cur->next;
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
	if (!handler)
		return(0);
	return(signal_purge_handlers(handler->parent, handler));
}

/**
 * Return the pointer to the handler from the list of handlers for the given
 * object and signal name that matches the given function (or if NULL is given
 * then any function) and that match the given pointer (or if NULL is given
 * then any pointer).
 */
struct signal_handler_s *signal_find_handler(void *obj, char *name, signal_t func, void *ptr)
{
	struct signal_s *signal;
	struct signal_list_s *list;
	struct signal_handler_s *cur;

	if (!obj)
		list = &signal_list;
	else if (!(list = object_find_node(&object_list, obj)))
		return(NULL);

	if (!(signal = signal_find_node(list, name)))
		return(NULL);
	cur = signal->handlers;
	while (cur) {
		if ((!func || (func == cur->func)) && (!ptr || (ptr == cur->ptr)))
			return(cur);
		cur = cur->next;
	}
	return(NULL);
}

/*** Local Functions ***/

/**
 * Create and initialize a signal struct.
 */
static inline struct signal_s *create_signal(struct signal_list_s *list, char *name, int bitflags)
{
	struct signal_s *signal;

	if (!(signal = signal_create_node(sizeof(struct signal_s) + strlen(name) + 1)))
		return(NULL);
	signal->name = (char *) offset_after_struct_m(signal, 0);
	strcpy(signal->name, name);
	signal->bitflags = bitflags;
	signal->handlers = NULL;
	signal_add_node(list, signal);
	return(signal);
}

/**
 * Destroy all the handlers associated with the given signal node.
 */
static inline void signal_release_node(struct signal_list_s *list, struct signal_s *signal)
{
	struct signal_handler_s *cur, *tmp;

	cur = signal->handlers;
	while (cur) {
		tmp = cur->next;
		memory_free(cur);
		cur = tmp;
	}
}

/**
 * Create a signal table for an object and add it to the object list.
 */
static inline struct signal_list_s *create_object(void *obj)
{
	struct signal_list_s *list;

	if (!(list = signal_create_table(SIGNAL_LIST_INIT_SIZE)))
		return(NULL);
	object_add_node(&object_list, list);
	return(list);
}

/**
 * Destroy all the signal lists associated with the given object.
 */
static inline void object_release_node(struct object_list_s *obj_list, struct signal_list_s *list)
{
	signal_destroy_table(list);
}

/**
 * Remove all handlers from the given node that have the purge flag set and the
 * given handler if found.
 */
static int signal_purge_handlers(struct signal_s *signal, struct signal_handler_s *handler)
{
	int disconnected = 0;
	struct signal_handler_s *cur, *prev, *tmp;

	prev = NULL;
	cur = signal->handlers;
	while (cur) {
		if ((cur->bitflags & SIG_HBF_PURGE) || (cur == handler)) {
			disconnected++;
			if (cur->bitflags & SIG_HBF_NODE_LOCKED) {
				cur->bitflags |= SIG_HBF_PURGE;
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


