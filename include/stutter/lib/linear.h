/*
 * Virtual Module Name:	linear.h
 * Version:		0.1
 * Description:		Linked List Virtual Interface
 */

#ifndef _LIB_LINEAR_H
#define _LIB_LINEAR_H

#include <lib/memory.h>
#include <lib/globals.h>

/*** Virtual Interface ***/

#define linear_node_v(nodetype)				\
	struct {					\
		struct nodetype *next;			\
		struct nodetype *prev;			\
	}

#define linear_list_v(nodetype)				\
	struct {					\
		struct nodetype *head;			\
	}

#define linear_init_v(list)				\
	(list).head = NULL;

#define linear_add_node_v(list, field, node) {		\
	(node)->field.prev = NULL;			\
	(node)->field.next = (list).head;		\
	if ((list).head)				\
		(list).head->field.prev = (node);	\
	(list).head = (node);				\
}

#define linear_remove_node_v(list, field, node) {			\
	if ((node)->field.prev)						\
		(node)->field.prev->field.next = (node)->field.next;	\
	else								\
		(list).head = (node)->field.next;			\
	if ((node)->field.next)						\
		(node)->field.next->field.prev = (node)->field.prev;	\
}

#define linear_find_node_v(list, field, ptr, compare) {	\
	typeof((list).head) cur;			\
	ptr = NULL;					\
	cur = (list).head;				\
	while (cur) {					\
		if (compare) {				\
			ptr = cur;			\
			break;				\
		}					\
		cur = cur->field.next;			\
	}						\
}							\

#define linear_traverse_list_v(list, field, body) {	\
	typeof((list).head) cur;			\
	cur = (list).head;				\
	while (cur) {					\
		body					\
		cur = cur->field.next;			\
	}						\
}

#define linear_destroy_list_v(list, field, body) {	\
	typeof((list).head) cur;			\
	typeof((list).head) tmp;			\
	cur = (list).head;				\
	while (cur) {					\
		tmp = cur->field.next;			\
		body					\
		cur = tmp;				\
	}						\
	(list).head = NULL;				\
}


/*** Inline Interface ***/

struct linear_node_s {
	void *ptr;
	linear_node_v(linear_node_s) list;
};

struct linear_list_s {
	linear_list_v(linear_node_s) list;
};

static inline struct linear_list_s *linear_create_list(void)
{
	struct linear_list_s *list;

	if (!(list = (struct linear_list_s *) memory_alloc(sizeof(struct linear_list_s))))
		return(NULL);
	linear_init_v(list->list);
	return(list);
}

static inline void linear_destroy_list(struct linear_list_s *list, destroy_t destroy)
{
	if (!list)
		return;
	linear_destroy_list_v(list->list, list,
		destroy(cur->ptr);
		memory_free(cur);
	);
	memory_free(list);
}

static inline int linear_add_node(struct linear_list_s *list, void *ptr)
{
	struct linear_node_s *node;

	if (!(node = (struct linear_node_s *) memory_alloc(sizeof(struct linear_node_s))))
		return(-1);
	node->ptr = ptr;
	linear_add_node_v(list->list, list, node);
	return(0);
}

static inline void *linear_remove_node(struct linear_list_s *list, void *ptr)
{
	void *ret;
	struct linear_node_s *node;

	linear_find_node_v(list->list, list, node, (cur->ptr == ptr));
	if (!node)
		return(NULL);
	ret = node->ptr;
	linear_remove_node_v(list->list, list, node);
	memory_free(node);
	return(ret);
}

static inline void *linear_find_node(struct linear_list_s *list, compare_t compare, void *ptr)
{
	struct linear_node_s *node;

	linear_find_node_v(list->list, list, node, (!compare(cur->ptr, ptr)));
	if (!node)
		return(NULL);
	return(node->ptr);
}


#endif

