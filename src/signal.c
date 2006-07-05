/*
 * Module Name:		signal.c
 * Version:		0.1
 * Module Requirements:	memory ; hash
 * Description:		Signal Manager
 */


#include <string.h>

#include CONFIG_H
#include <stutter/signal.h>
#include <stutter/lib/hash.h>
#include <stutter/lib/memory.h>
#include <stutter/lib/globals.h>

#ifndef SIGNAL_INIT_SIZE
#define SIGNAL_INIT_SIZE	10
#endif

#ifndef SIGNAL_LOAD_FACTOR
#define SIGNAL_LOAD_FACTOR	0.75
#endif

#define signal_hash_m(list, str)		(sdbm_hash(str) % hash_size_v(list))
#define signal_compare_m(str)			(!strcmp(cur->data.name, str))

struct signal_node_s {
	struct signal_s data;
	hash_node_v(signal_node_s) sl;
};

static int signal_initialized = 0;
static hash_list_v(signal_node_s) signal_list;

static void signal_destroy_node(struct signal_node_s *);

int init_signal(void)
{
	if (signal_initialized)
		return(1);
	hash_init_v(signal_list, SIGNAL_INIT_SIZE);
	signal_initialized = 1;
	return(0);
}

int release_signal(void)
{
	if (!signal_initialized)
		return(1);
	signal_initialized = 0;
	hash_destroy_list_v(signal_list, sl,
		signal_destroy_node(cur);
	);
	hash_release_v(signal_list);
	return(0);
}

/**
 * Add a new signal with the given name to the list of signals.
 */
int add_signal(char *name, int bitflags)
{
	struct signal_node_s *node;

	if (!(node = memory_alloc(sizeof(struct signal_node_s) + strlen(name) + 1)))
		return(-1);
	node->data.name = (char *) (((unsigned int) node) + sizeof(struct signal_node_s));
	strcpy(node->data.name, name);
	node->data.bitflags = bitflags;
	node->data.handlers = NULL;
	hash_add_node_v(signal_list, sl, node, signal_hash_m(signal_list, name));
	if (hash_load_v(signal_list) > SIGNAL_LOAD_FACTOR)
		hash_rehash_v(signal_list, sl, (hash_size_v(signal_list) * 1.75), signal_hash_m(signal_list, node->data.name));
	return(0);
}

/**
 * Remove the signal with the given name and destroy all handlers associated
 * with that signal.  If the signal cannot be found, -1 is returned,
 * otherwise 0 is returned.
 */
int remove_signal(char *name)
{
	struct signal_node_s *node;

	hash_remove_node_v(signal_list, sl, node, signal_hash_m(signal_list, name), signal_compare_m(name));
	if (!node)
		return(-1);
	signal_destroy_node(node);
	return(0);
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
	hash_find_node_v(signal_list, sl, node, signal_hash_m(signal_list, name), signal_compare_m(name));
	if (!node)
		return(-1);
	if (!(handler = memory_alloc(sizeof(struct signal_handler_s))))
		return(-1);
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
				break;
			}
			else if (!cur->next)
				cur->next = handler;
			prev = cur;
			cur = cur->next;
		}
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
	int disconnected = 0;
	struct signal_node_s *node;
	struct signal_handler_s *cur, *prev, *tmp;

	hash_find_node_v(signal_list, sl, node, signal_hash_m(signal_list, name), signal_compare_m(name));
	if (!node)
		return(-1);
	prev = NULL;
	cur = node->data.handlers;
	while (cur) {
		if ((cur->index == index) && (!func || (cur->func == func)) && (!ptr || (cur->ptr == ptr))) {
			if (prev)
				prev->next = tmp = cur->next;
			else
				node->data.handlers = tmp = cur->next;
			memory_free(cur);
			disconnected++;
			cur = tmp;
		}
		else {
			prev = cur;
			cur = cur->next;
		}
	}
	return(disconnected);
}

/**
 * All of the handlers associated with the signal specified by the given
 * name are called with the given pointer passed as the second argument.
 * The number of handlers called is returned or -1 is returned if the
 * signal is not found.
 */
int signal_emit(char *name, void *index, void *ptr)
{
	int calls = 0;
	struct signal_node_s *node;
	struct signal_handler_s *cur;

	hash_find_node_v(signal_list, sl, node, signal_hash_m(signal_list, name), signal_compare_m(name));
	if (!node)
		return(-1);
	cur = node->data.handlers;
	while (cur) {
		if (cur->index == index) {
			// TODO change the signal function format
			//cur->func(cur->ptr, index, ptr)
			calls++;
			if (cur->func(cur->ptr, ptr) == SIGNAL_STOP_EMIT)
				break;
		}
		cur = cur->next;
	}
	return(calls);
}


/*** Local Functions ***/

/**
 * Destroy all the handlers associated with the given signal node as
 * well as the node itself.
 */
static void signal_destroy_node(struct signal_node_s *node)
{
	struct signal_handler_s *cur, *tmp;

	cur = node->data.handlers;
	while (cur) {
		tmp = cur->next;
		memory_free(tmp);
		cur = tmp;
	}
	memory_free(node);
}

