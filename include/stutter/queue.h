/*
 * Virtual Module Name:	queue.h
 * Description:		Queue Virtual Interface
 */

#ifndef _STUTTER_QUEUE_H
#define _STUTTER_QUEUE_H

#include <stutter/memory.h>
#include <stutter/globals.h>

/*** Virtual Interface ***/

#define queue_node_v(nodetype)				\
	struct {					\
		struct nodetype *next;			\
		struct nodetype *prev;			\
	}

#define queue_list_v(nodetype)				\
	struct {					\
		int size;				\
		int max;				\
		struct nodetype *current;		\
		struct nodetype *head;			\
		struct nodetype *tail;			\
	}

#define queue_init_v(list, max_size)			\
	(list).size = 0;				\
	(list).max = max_size;				\
	(list).current = NULL;				\
	(list).head = NULL;				\
	(list).tail = NULL;

#define queue_release_v(list)				\
	(list).size = 0;				\
	(list).current = NULL;				\
	(list).head = NULL;				\
	(list).tail = NULL;

#define queue_prepend_node_v(list, field, node) {	\
	(node)->field.next = (list).head;		\
	(node)->field.prev = NULL;			\
	if ((list).head)				\
		(list).head->field.prev = (node);	\
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

#define queue_insert_node_v(list, field, after_node, node) {	\
	(node)->field.prev = (after_node);			\
	(node)->field.next = (after_node)->field.next;		\
	if ((after_node)->field.next)				\
		(after_node)->field.next->field.prev = (node);	\
	(after_node)->field.next = (node);			\
	if ((list).tail == (after_node))			\
		(list).tail = (node);				\
	(list).size++;						\
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
		if ((list).current == ptr)		\
			(list).current = NULL;		\
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
		if ((list).current == ptr)		\
			(list).current = NULL;		\
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
	if ((list).current == node)					\
		(list).current = NULL;					\
}

#define queue_foreach_v(list, field, cur)		\
	for (cur = (list).head;cur;cur = cur->field.next)

#define queue_foreach_safe_v(list, field, cur, tmp)	\
	for (cur = (list).head;cur && ((tmp = cur->field.next), 1);cur = tmp)

#define queue_reverse_foreach_v(list, field, cur)		\
	for (cur = (list).tail;cur;cur = cur->field.prev)

#define queue_reverse_foreach_safe_v(list, field, cur, tmp)	\
	for (cur = (list).tail;cur && ((tmp = cur->field.prev), 1);cur = tmp)

#define queue_first_v(list)		( (list).head )
#define queue_last_v(list)		( (list).tail )
#define queue_next_v(field, node)	( (node)->field.next )
#define queue_previous_v(field, node)	( (node)->field.prev )
#define queue_current_v(list)		( (list).current )
#define queue_set_current_v(list, node)	( (list).current = node )
#define queue_size_v(list)		( (list).size )
#define queue_max_v(list)		( (list).max )

/*** Inline Interface ***/

#define DEFINE_QUEUE_INIT_LIST(queue_name, list_type, node_type, field)	\
	static inline int queue_name##_init_list(list_type *queue, int max) {	\
		queue_init_v(queue->field, max);				\
		return(0);							\
	}

#define DEFINE_QUEUE_RELEASE_LIST(queue_name, list_type, node_type, field, release)	\
	static inline int queue_name##_release_list(list_type *queue) {			\
		node_type *cur, *tmp;							\
											\
		queue_foreach_safe_v(queue->field, field, cur, tmp) {			\
			release(queue, cur);						\
			memory_free(cur);						\
		}									\
		queue_release_v(queue->field);						\
		return(0);								\
	}

#define DEFINE_QUEUE_CREATE_LIST(queue_name, list_type, node_type, field)	\
	static inline list_type *queue_name##_create_list(int max) {		\
		list_type *queue;						\
										\
		if (!(queue = (list_type *) memory_alloc(sizeof(list_type))))	\
			return(NULL);						\
		queue_init_v(queue->field, max);				\
		return(queue);							\
	}

#define DEFINE_QUEUE_DESTROY_LIST(queue_name, list_type, node_type, field, release)	\
	static inline int queue_name##_destroy_list(list_type *queue) {			\
		node_type *cur, *tmp;							\
											\
		queue_foreach_safe_v(queue->field, field, cur, tmp) {			\
			release(queue, cur);						\
			memory_free(cur);						\
		}									\
		memory_free(queue);							\
		return(0);								\
	}

#define DEFINE_QUEUE_CREATE_NODE(queue_name, list_type, node_type, field)					\
	static inline node_type *queue_name##_create_node(int size) {				\
		node_type *node;								\
												\
		if (!(node = (node_type *) memory_alloc(size ? size : sizeof(node_type))))	\
			return(NULL);								\
		return(node);									\
	}

#define DEFINE_QUEUE_DESTROY_NODE(queue_name, list_type, node_type, field, release)		\
	static inline int queue_name##_destroy_node(list_type *queue, node_type *node) {	\
		release(queue, node);								\
		memory_free(node);								\
		return(0);									\
	}

#define DEFINE_QUEUE_PREPEND_NODE(queue_name, list_type, node_type, field, release)		\
	static inline int queue_name##_prepend_node(list_type *queue, node_type *node) {	\
		queue_prepend_node_v(queue->field, field, node);				\
		if (queue_max_v(queue->field) && (queue_size_v(queue->field) > queue_max_v(queue->field))) {	\
			queue_pop_node_v(queue->field, field, node);				\
			if (queue_current_v(queue->field) == node)				\
				queue_set_current_v(queue->field, NULL);			\
			release(queue, node);							\
			memory_free(node);							\
		}										\
		return(0);									\
	}

#define DEFINE_QUEUE_APPEND_NODE(queue_name, list_type, node_type, field)			\
	static inline int queue_name##_append_node(list_type *queue, node_type *node) {		\
		if (queue_max_v(queue->field) && (queue_size_v(queue->field) >= queue_max_v(queue->field)))	\
			return(1);								\
		queue_append_node_v(queue->field, field, node);					\
		return(0);									\
	}

#define DEFINE_QUEUE_INSERT_NODE(queue_name, list_type, node_type, field)			\
	static inline int queue_name##_insert_node(list_type *queue, node_type *after_node, node_type *node) {		\
		if (queue_max_v(queue->field) && (queue_size_v(queue->field) >= queue_max_v(queue->field)))	\
			return(1);								\
		queue_insert_node_v(queue->field, field, after_node, node);			\
		return(0);									\
	}

#define DEFINE_QUEUE_SHIFT_NODE(queue_name, list_type, node_type, field)	\
	static inline node_type *queue_name##_shift_node(list_type *queue) {	\
		node_type *node;						\
										\
		queue_shift_node_v(queue->field, field, node);			\
		if (queue_current_v(queue->field) == node)			\
			queue_set_current_v(queue->field, NULL);		\
		return(node);							\
	}

#define DEFINE_QUEUE_POP_NODE(queue_name, list_type, node_type, field)		\
	static inline node_type *queue_name##_pop_node(list_type *queue) {	\
		node_type *node;						\
										\
		queue_pop_node_v(queue->field, field, node);			\
		if (queue_current_v(queue->field) == node)			\
			queue_set_current_v(queue->field, NULL);		\
		return(node);							\
	}

#define DEFINE_QUEUE_REMOVE_NODE(queue_name, list_type, node_type, field)			\
	static inline void queue_name##_remove_node(list_type *queue, node_type *node) {	\
		if (queue_current_v(queue->field) == node)					\
			queue_set_current_v(queue->field, NULL);				\
		queue_remove_node_v(queue->field, field, node);					\
	}

#define DEFINE_QUEUE_DELETE_NODE(queue_name, list_type, node_type, field, release)		\
	static inline void queue_name##_delete_node(list_type *queue, node_type *node) {	\
		if (queue_current_v(queue->field) == node)					\
			queue_set_current_v(queue->field, NULL);				\
		queue_remove_node_v(queue->field, field, node);					\
		release(queue, node);								\
		memory_free(node);								\
	}

#define DEFINE_QUEUE_FIND_NODE(queue_name, list_type, node_type, field, compare)	\
	static inline node_type *queue_name##_find_node(list_type *queue, void *ptr) {	\
		node_type *cur;								\
											\
		queue_foreach_v(queue->field, field, cur) {				\
			if (compare(cur, ptr))						\
				return(cur);						\
		}									\
		return(NULL);								\
	}

#define DEFINE_QUEUE_FIRST_NODE(queue_name, list_type, node_type, field)	\
	static inline node_type *queue_name##_first_node(list_type *queue) {	\
		return(queue_first_v(queue->field));				\
	}

#define DEFINE_QUEUE_LAST_NODE(queue_name, list_type, node_type, field)		\
	static inline node_type *queue_name##_last_node(list_type *queue) {	\
		return(queue_last_v(queue->field));				\
	}

#define DEFINE_QUEUE_NEXT_NODE(queue_name, list_type, node_type, field)		\
	static inline node_type *queue_name##_next_node(node_type *node) {	\
		return(node ? queue_next_v(field, node) : NULL);		\
	}

#define DEFINE_QUEUE_PREVIOUS_NODE(queue_name, list_type, node_type, field)	\
	static inline node_type *queue_name##_previous_node(node_type *node) {	\
		return(node ? queue_previous_v(field, node) : NULL);		\
	}

#define DEFINE_QUEUE_CURRENT_NODE(queue_name, list_type, node_type, field)	\
	static inline node_type *queue_name##_current_node(list_type *queue) {	\
		return(queue_current_v(queue->field));				\
	}

#define DEFINE_QUEUE_SET_CURRENT_NODE(queue_name, list_type, node_type, field)				\
	static inline node_type *queue_name##_set_current_node(list_type *queue, node_type *node) {	\
		return(queue_set_current_v(queue->field, node));					\
	}

#define DEFINE_QUEUE_SIZE(queue_name, list_type, node_type, field)	\
	static inline int queue_name##_size(list_type *queue) {		\
		return(queue_size_v(queue->field));			\
	}

#define DEFINE_QUEUE_MAX(queue_name, list_type, node_type, field)	\
	static inline int queue_name##_max(list_type *queue) {		\
		return(queue_max_v(queue->field));			\
	}

#define DEFINE_QUEUE(queue_name, list_type, node_type, field, release, compare)		\
	DEFINE_QUEUE_INIT_LIST(queue_name, list_type, node_type, field)			\
	DEFINE_QUEUE_RELEASE_LIST(queue_name, list_type, node_type, field, release)	\
	DEFINE_QUEUE_CREATE_LIST(queue_name, list_type, node_type, field)		\
	DEFINE_QUEUE_DESTROY_LIST(queue_name, list_type, node_type, field, release)	\
	DEFINE_QUEUE_CREATE_NODE(queue_name, list_type, node_type, field)		\
	DEFINE_QUEUE_DESTROY_NODE(queue_name, list_type, node_type, field, release)	\
	DEFINE_QUEUE_PREPEND_NODE(queue_name, list_type, node_type, field, release)	\
	DEFINE_QUEUE_APPEND_NODE(queue_name, list_type, node_type, field)		\
	DEFINE_QUEUE_INSERT_NODE(queue_name, list_type, node_type, field)		\
	DEFINE_QUEUE_SHIFT_NODE(queue_name, list_type, node_type, field)		\
	DEFINE_QUEUE_POP_NODE(queue_name, list_type, node_type, field)			\
	DEFINE_QUEUE_REMOVE_NODE(queue_name, list_type, node_type, field)		\
	DEFINE_QUEUE_DELETE_NODE(queue_name, list_type, node_type, field, release)	\
	DEFINE_QUEUE_FIND_NODE(queue_name, list_type, node_type, field, compare)	\
	DEFINE_QUEUE_FIRST_NODE(queue_name, list_type, node_type, field)		\
	DEFINE_QUEUE_LAST_NODE(queue_name, list_type, node_type, field)			\
	DEFINE_QUEUE_NEXT_NODE(queue_name, list_type, node_type, field)			\
	DEFINE_QUEUE_PREVIOUS_NODE(queue_name, list_type, node_type, field)		\
	DEFINE_QUEUE_CURRENT_NODE(queue_name, list_type, node_type, field)		\
	DEFINE_QUEUE_SET_CURRENT_NODE(queue_name, list_type, node_type, field)		\
	DEFINE_QUEUE_SIZE(queue_name, list_type, node_type, field)			\
	DEFINE_QUEUE_MAX(queue_name, list_type, node_type, field)

/*** Predefined Generic Queue Interface ***/

struct queue_node_s {
	queue_node_v(queue_node_s) q;
	void *ptr;
};

struct queue_s {
	queue_list_v(queue_node_s) q;
	destroy_t destroy;
};

static inline int queue_init_node(struct queue_s *queue, struct queue_node_s *node, void *ptr)
{
	node->ptr = ptr;
	return(0);
}

static inline int queue_release_node(struct queue_s *queue, struct queue_node_s *node)
{
	if (queue->destroy)
		queue->destroy(node->ptr);
	return(0);
}

#define queue_compare_node(node, ptr)	\
	(node->ptr == ptr)

DEFINE_QUEUE(queue, struct queue_s, struct queue_node_s, q, queue_release_node, queue_compare_node)

static inline struct queue_s *create_queue(int max, destroy_t destroy)
{
	struct queue_s *queue;

	if (!(queue = queue_create_list(max)))
		return(NULL);
	queue->destroy = destroy;
	return(queue);
}

static inline int destroy_queue(struct queue_s *queue)
{
	return(queue_destroy_list(queue));
}

#define queue_foreach(list, cur)	\
	queue_foreach_v(list->q, q, cur)

#define queue_foreach_safe(list, cur, tmp)	\
	queue_foreach_safe_v(list->q, q, cur, tmp)

#define queue_reverse_foreach(list, cur)	\
	queue_reverse_foreach_v(list->q, q, cur)

#define queue_reverse_foreach_safe(list, cur, tmp)	\
	queue_reverse_foreach_safe_v(list->q, q, cur, tmp)

#endif

