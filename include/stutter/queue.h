/*
 * Module Name:		queue.h
 * Description:		Queue Header
 */

#ifndef _STUTTER_QUEUE_H
#define _STUTTER_QUEUE_H

#include <stutter/memory.h>
#include <stutter/globals.h>

struct queue_entry_s {
	void *data;
	struct queue_entry_s *next;
	struct queue_entry_s *prev;
};

struct queue_s {
	int max;
	int size;
	destroy_t destroy;
	struct queue_entry_s *current;
	struct queue_entry_s *head;
	struct queue_entry_s *tail;
};

struct queue_s *create_queue(int, destroy_t);
void destroy_queue(struct queue_s *);

int queue_prepend(struct queue_s *, void *);
int queue_append(struct queue_s *, void *);
int queue_insert(struct queue_s *, void *);
void *queue_shift(struct queue_s *);
void *queue_pop(struct queue_s *);
void *queue_remove(struct queue_s *);
void queue_delete(struct queue_s *);
void queue_clear(struct queue_s *);

#define QUEUE_CURRENT_ENTRY(queue)	\
	( (queue)->current )

#define QUEUE_SET_CURRENT_ENTRY(queue, entry)	\
	( (queue)->current = (entry) )

#define QUEUE_FIRST_ENTRY(queue)	\
	( (queue)->current = (queue)->head )

#define QUEUE_LAST_ENTRY(queue)		\
	( (queue)->current = (queue)->tail )

#define QUEUE_NEXT_ENTRY(queue)		\
	( (queue)->current = ( (queue)->current ? (queue)->current->next : NULL ) )

#define QUEUE_PREV_ENTRY(queue)		\
	( (queue)->current = ( (queue)->current ? (queue)->current->prev : NULL ) )


#define QUEUE_CURRENT(queue)	\
	( (queue)->current ? (queue)->current->data : NULL )

#define QUEUE_MAX(queue)	\
	( (queue)->max )

#define QUEUE_SIZE(queue)	\
	( (queue)->size )

#define QUEUE_FOREACH(queue)	\
	for (QUEUE_FIRST_ENTRY(queue); QUEUE_CURRENT_ENTRY(queue); QUEUE_NEXT_ENTRY(queue))

#define QUEUE_FOREACH_REVERSE(queue)	\
	for (QUEUE_LAST_ENTRY(queue); QUEUE_CURRENT_ENTRY(queue); QUEUE_PREV_ENTRY(queue))

#endif

