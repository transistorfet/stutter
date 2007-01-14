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
#include <stutter/globals.h>

#define signal_hash(list, key)	\
	sdbm_hash_icase(key)

#define signal_compare_node(node, ptr)	\
	(!strcmp_icase(node->data.name, key))

struct signal_node_s {
	struct signal_s data;
	hash_node_v(signal_node_s) sl;
};

struct signal_list_s {
	hash_list_v(signal_node_s) sl;
};

static inline void signal_release_node(struct signal_list_s *, struct signal_node_s *);

DEFINE_HASH_TABLE(signal, struct signal_list_s, struct signal_node_s, sl, data.name, signal_release_node, signal_hash, signal_compare_node, SIGNAL_LIST_LOAD_FACTOR, SIGNAL_LIST_GROWTH_FACTOR)

static int signal_initialized = 0;
static struct signal_list_s signal_list;

static int signal_remove_handlers(struct signal_node_s *, int, void *, signal_t, void *);

int init_signal(void)
{
	if (signal_initialized)
		return(1);
	signal_init_table(&signal_list, SIGNAL_LIST_INIT_SIZE);
	signal_initialized = 1;
	return(0);
}

int release_signal(void)
{
	if (!signal_initialized)
		return(1);
	signal_release_table(&signal_list);
	signal_initialized = 0;
	return(0);
}

/**
 * Add a new signal with the given name to the list of signals.
 */
int add_signal(char *name, int bitflags)
{
	struct signal_node_s *node;

	if (!(node = signal_create_node(sizeof(struct signal_node_s) + strlen(name) + 1)))
		return(-1);
	node->data.name = (char *) (((unsigned int) node) + sizeof(struct signal_node_s));
	strcpy(node->data.name, name);
	node->data.bitflags = bitflags;
	node->data.handlers = NULL;
	signal_add_node(&signal_list, node);
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
 * All of the handlers associated with the signal specified by the given
 * name are called with the given pointer passed as the second argument.
 * The number of handlers called is returned or -1 is returned if the
 * signal is not found.
 */
int emit_signal(char *name, void *index, void *ptr)
{
	int ret;
	int calls = 0, purge = 0;
	struct signal_node_s *node;
	struct signal_handler_s *cur;

	if (!(node = signal_find_node(&signal_list, name)))
		return(-1);
	cur = node->data.handlers;
	while (cur) {
		if (cur->index == index || (!cur->index && (node->data.bitflags & SIG_BF_USE_WILDCARD_INDEX))) {
			calls++;
			cur->bitflags |= SIG_HBF_NODE_LOCKED;
			ret = cur->func(cur->ptr, index, ptr);
			cur->bitflags &= ~SIG_HBF_NODE_LOCKED;
			if (cur->bitflags & SIG_HBF_PURGE)
				purge = 1;
			if (ret == SIGNAL_STOP_EMIT)
				break;
		}
		cur = cur->next;
	}
	if (purge)
		signal_remove_handlers(node, 0, NULL, NULL, NULL);
	return(calls);
}

/**
 * Connect a new handler to the signal with the given name so that the given
 * when that signal is emitted, the given function is called with the given
 * pointer passed as the first argument.  The given function must not be NULL
 * and the signal must already be created using the add_signal function.  If
 * an error occurs, -1 is returned, otherwise 0 is returned. 
 */
int signal_connect(char *name, void *index, int priority, signal_t func, void *ptr)
{
	struct signal_node_s *node;
	struct signal_handler_s *handler, *cur, *prev;

	if (!func)
		return(-1);
	if (!(node = signal_find_node(&signal_list, name)))
		return(-1);
	if (!(handler = memory_alloc(sizeof(struct signal_handler_s))))
		return(-1);
	handler->bitflags = 0;
	handler->index = index;
	handler->priority = priority;
	handler->func = func;
	handler->ptr = ptr;
	handler->next = NULL;

	if (!node->data.handlers)
		node->data.handlers = handler;
	else {
		prev = NULL;
		cur = node->data.handlers;
		while (cur) {
			if (priority >= cur->priority) {
				if (prev) {
					prev->next = handler;
					handler->next = cur;
				}
				else {
					handler->next = node->data.handlers;
					node->data.handlers = handler;
				}
				return(0);
			}
			prev = cur;
			cur = cur->next;
		}
		/** The handler wasn't added so it should be added to the end */
		prev->next = handler;
		handler->next = NULL;
	}
	return(0);
}

/**
 * Remove all handlers from the list of handlers for the given signal name
 * that match the given function (or if NULL is given then any function)
 * and that match the given pointer (or if NULL is given then any pointer).
 * The number of signals disconnected is returned.
 */
int signal_disconnect(char *name, void *index, signal_t func, void *ptr)
{
	struct signal_node_s *node;

	if (!(node = signal_find_node(&signal_list, name)))
		return(-1);
	return(signal_remove_handlers(node, 1, index, func, ptr));
}


/*** Local Functions ***/

/**
 * Destroy all the handlers associated with the given signal node as
 * well as the node itself.
 */
static inline void signal_release_node(struct signal_list_s *table, struct signal_node_s *node)
{
	struct signal_handler_s *cur, *tmp;

	cur = node->data.handlers;
	while (cur) {
		tmp = cur->next;
		memory_free(cur);
		cur = tmp;
	}
}

/**
 * Remove all handlers from the given node that have the purge flag set or
 * if match is non-zero, that have the given index, the given function
 * (or if NULL is given then any function), and that match the given pointer
 * (or if NULL is given then any pointer).  The number of handlers remove is
 * returned.
 */
static int signal_remove_handlers(struct signal_node_s *node, int match, void *index, signal_t func, void *ptr)
{
	int disconnected = 0;
	struct signal_handler_s *cur, *prev, *tmp;

	prev = NULL;
	cur = node->data.handlers;
	while (cur) {
		if ((cur->bitflags & SIG_HBF_PURGE) || (match && (cur->index == index) && (!func || (cur->func == func)) && (!ptr || (cur->ptr == ptr)))) {
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
					node->data.handlers = tmp = cur->next;
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

