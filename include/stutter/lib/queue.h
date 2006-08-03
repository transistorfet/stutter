/*
 * Virtual Module Name:	queue.h
 * Version:		0.1
 * Description:		Queue Virtual Interface
 */

#ifndef _STUTTER_LIB_QUEUE_H
#define _STUTTER_LIB_QUEUE_H

#include <stutter/lib/memory.h>
#include <stutter/lib/globals.h>

/*** Virtual Interface ***/

#define queue_node_v(nodetype)				\
	struct {					\
		struct nodetype *next;			\
		struct nodetype *prev;			\
	}

#define queue_list_v(nodetype)				\
	struct {					\
		int size;				\
		struct nodetype *head;			\
		struct nodetype *tail;			\
	}

#define queue_init_v(list)				\
	(list).size = 0;				\
	(list).head = NULL;				\
	(list).tail = NULL;

#define queue_prepend_node_v(list, field, node) {	\
	(node)->field.next = (list).head;		\
	(node)->field.prev = NULL;			\
	(list).head = (node);				\
	if (!(list).tail)				\
		(list).tail = (node);			\
	(list).size++;					\
}

#define queue_append_node_v(list, field, node) {	\
	(node)->field.next = NULL;			\
	(node)->field.prev = (list).tail;		\
	if ((list).tail)				\
		(list).tail->field.next = (node);	\
	else						\
		(list).head = (node);			\
	(list).tail = (node);				\
	(list).size++;					\
}

#define queue_shift_node_v(list, field, ptr) {		\
	ptr = (list).head;				\
	if ((list).head) {				\
		(list).head = (list).head->field.next;	\
		if ((list).head)			\
			(list).head->field.prev = NULL;	\
		else					\
			(list).tail = NULL;		\
		(list).size--;				\
	}						\
}

#define queue_pop_node_v(list, field, ptr) {		\
	ptr = (list).tail;				\
	if ((list).tail) {				\
		(list).tail = (list).tail->field.prev;	\
		if ((list).tail)			\
			(list).tail->field.next = NULL;	\
		else					\
			(list).head = NULL;		\
		(list).size--;				\
	}						\
}

#define queue_remove_node_v(list, field, node) {			\
	if ((node)->field.prev)						\
		(node)->field.prev->field.next = (node)->field.next;	\
	else								\
		(list).head = (node)->field.next;			\
	if ((node)->field.next)						\
		(node)->field.next->field.prev = (node)->field.prev;	\
	else								\
		(list).tail = (node)->field.prev;			\
}

#define queue_destroy_v(list, field, body) {		\
	typeof((list).head) cur;			\
	typeof((list).head) tmp;			\
	cur = (list).head;				\
	while (cur) {					\
		tmp = cur->field.next;			\
		body					\
		cur = tmp;				\
	}						\
	(list).head = NULL;				\
	(list).tail = NULL;				\
	(list).size = 0;				\
}

#define queue_traverse_v(list, field, body) {		\
	typeof((list).head) cur;			\
	cur = (list).head;				\
	while (cur) {					\
		body					\
		cur = cur->field.next;			\
	}						\
}

#define queue_first_v(list)		(list).head
#define queue_last_v(list)		(list).tail
#define queue_next_v(field, node)	(node)->field.next
#define queue_previous_v(field, node)	(node)->field.prev
#define queue_size_v(list)		(list).size

/*** Inline Interface ***/

struct queue_node_s {
	void *ptr;
	queue_node_v(queue_node_s) q;
};

struct queue_s {
	int max;
	destroy_t destroy;
	struct queue_node_s *current;
	queue_list_v(queue_node_s) q;
};

static inline struct queue_s *create_queue(int max, destroy_t destroy)
{
	struct queue_s *queue;

	if (!(queue = (struct queue_s *) memory_alloc(sizeof(struct queue_s))))
		return(NULL);
	queue->max = max;
	queue->destroy = destroy;
	queue->current = NULL;
	queue_init_v(queue->q);
	return(queue);
}

static inline void destroy_queue(struct queue_s *queue)
{
	queue_destroy_v(queue->q, q,
		queue->destroy(cur->ptr);
		memory_free(cur);
	);
	memory_free(queue);
}

static inline int queue_prepend(struct queue_s *queue, void *ptr)
{
	struct queue_node_s *node;

	if (!(node = (struct queue_node_s *) memory_alloc(sizeof(struct queue_node_s))))
		return(-1);
	node->ptr = ptr;
	queue_prepend_node_v(queue->q, q, node);
	if (queue->max && (queue_size_v(queue->q) > queue->max)) {
		queue_pop_node_v(queue->q, q, node);
		if (queue->current == node)
			queue->current = NULL;
		queue->destroy(node->ptr);
		memory_free(node);
	}
	return(0);
}

static inline int queue_append(struct queue_s *queue, void *ptr)
{
	struct queue_node_s *node;

	if (queue->max && (queue_size_v(queue->q) + 1 > queue->max))
		return(1);
	if (!(node = (struct queue_node_s *) memory_alloc(sizeof(struct queue_node_s))))
		return(-1);
	node->ptr = ptr;
	queue_append_node_v(queue->q, q, node);
	return(0);
}

static inline void *queue_shift(struct queue_s *queue)
{
	void *ptr;
	struct queue_node_s *node;

	queue_shift_node_v(queue->q, q, node);
	ptr = node->ptr;
	if (queue->current == node)
		queue->current = NULL;
	memory_free(node);
	return(ptr);
}

static inline void *queue_pop(struct queue_s *queue)
{
	void *ptr;
	struct queue_node_s *node;

	queue_pop_node_v(queue->q, q, node);
	ptr = node->ptr;
	if (queue->current == node)
		queue->current = NULL;
	memory_free(node);
	return(ptr);
}

static inline void queue_delete(struct queue_s *queue, void *ptr)
{
	queue_traverse_v(queue->q, q,
		if (cur->ptr == ptr) {
			if (queue->current == cur)
				queue->current = NULL;
			queue_remove_node_v(queue->q, q, cur);
			break;
		}
	);
}

static inline struct queue_node_s *queue_find(struct queue_s *queue, void *ptr)
{
	queue_traverse_v(queue->q, q,
		if (cur->ptr == ptr)
			return(cur);
	);
	return(NULL);
}

#define queue_first(list)		( ((list) && ((list)->current = (list)->q.head)) ? (list)->current->ptr : NULL )
#define queue_last(list)		( ((list) && ((list)->current = (list)->q.tail)) ? (list)->current->ptr : NULL )
#define queue_next(list)		( ((list) && (list)->current && ((list)->current = (list)->current->q.next)) ? (list)->current->ptr : NULL )
#define queue_previous(list)		( ((list) && (list)->current && ((list)->current = (list)->current->q.prev)) ? (list)->current->ptr : NULL )
#define queue_current(list)		( ((list) && (list)->current) ? ((list)->current)->ptr : NULL )

#define queue_first_node(list)		( (list) ? ((list)->current = (list)->q.head) : NULL )
#define queue_last_node(list)		( (list) ? ((list)->current = (list)->q.tail) : NULL )
#define queue_next_node(list)		( ((list) && (list)->current) ? ((list)->current = (list)->current->q.next) : NULL )
#define queue_previous_node(list)	( ((list) && (list)->current) ? ((list)->current = (list)->current->q.prev) : NULL )
#define queue_current_node(list)	( (list) ? (list)->current : NULL )

#define queue_size(list)		( (list) ? (list)->q.size : NULL )

#endif

