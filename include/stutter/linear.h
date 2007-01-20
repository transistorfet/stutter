/*
 * Virtual Module Name:	linear.h
 * Version:		0.1
 * Description:		Linked List Virtual Interface
 */

#ifndef _STUTTER_LINEAR_H
#define _STUTTER_LINEAR_H

#include <stutter/memory.h>
#include <stutter/globals.h>

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

#define linear_release_v(list)				\
	(list).head = NULL;

#define linear_add_node_v(list, field, node) {		\
	(node)->field.prev = NULL;			\
	(node)->field.next = (list).head;		\
	if ((list).head)				\
		(list).head->field.prev = (node);	\
	(list).head = (node);				\
}

#define linear_add_node_sorted_v(list, field, node, compare) {		\
	typeof((list).head) cur;					\
	typeof((list).head) prev;					\
	prev = NULL;							\
	cur = (list).head;						\
	while (cur) {							\
		if (compare)						\
			break;						\
		prev = cur;						\
		cur = cur->field.next;					\
	}								\
	(node)->field.prev = prev;					\
	(node)->field.next = cur;					\
	if (prev)							\
		prev->field.next = (node);				\
	else								\
		(list).head = (node);					\
	if (cur)							\
		cur->field.prev = (node);				\
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

#define linear_foreach_v(list, field, cur)		\
	for (cur = (list).head;cur;cur = cur->field.next)

#define linear_foreach_safe_v(list, field, cur, tmp)	\
	for (cur = (list).head;cur && ((tmp = cur->field.next), 1);cur = tmp)

#define linear_first_v(list)		( (list).head )
#define linear_next_v(field, node)	( (node)->field.next )
#define linear_previous_v(field, node)	( (node)->field.prev )

/*** Inline Interface ***/

#define DEFINE_LINEAR_INIT_LIST(linear_name, list_type, node_type, field)	\
	static inline int linear_name##_init_list(list_type *list) {		\
		linear_init_v(list->field);					\
		return(0);							\
	}

#define DEFINE_LINEAR_RELEASE_LIST(linear_name, list_type, node_type, field, release)	\
	static inline int linear_name##_release_list(list_type *list) {			\
		node_type *cur, *tmp;							\
											\
		linear_foreach_safe_v(list->field, field, cur, tmp) {			\
			release(list, cur);						\
			memory_free(cur);						\
		}									\
		linear_release_v(list->field);						\
		return(0);								\
	}

#define DEFINE_LINEAR_CREATE_LIST(linear_name, list_type, node_type, field)	\
	static inline list_type *linear_name##_create_list(void) {		\
		list_type *list;						\
										\
		if (!(list = (list_type *) memory_alloc(sizeof(list_type))))	\
			return(NULL);						\
		linear_init_v(list->field);					\
		return(list);							\
	}

#define DEFINE_LINEAR_DESTROY_LIST(linear_name, list_type, node_type, field, release)	\
	static inline void linear_name##_destroy_list(list_type *list) {		\
		node_type *cur, *tmp;							\
											\
		linear_foreach_safe_v(list->field, field, cur, tmp) {			\
			release(list, cur);						\
			memory_free(cur);						\
		}									\
		memory_free(list);							\
	}

#define DEFINE_LINEAR_CREATE_NODE(linear_name, list_type, node_type, field)			\
	static inline node_type *linear_name##_create_node(int size) {				\
		node_type *node;								\
												\
		if (!(node = (node_type *) memory_alloc(size ? size : sizeof(node_type))))	\
			return(NULL);								\
		return(node);									\
	}

#define DEFINE_LINEAR_DESTROY_NODE(linear_name, list_type, node_type, field, release)		\
	static inline list_type *linear_name##_destroy_node(list_type *list, node_type *node) {	\
		release(list, node);								\
		memory_free(node);								\
		return(0);									\
	}

#define DEFINE_LINEAR_ADD_NODE(linear_name, list_type, node_type, field)		\
	static inline int linear_name##_add_node(list_type *list, node_type *node) {	\
		linear_add_node_v(list->field, field, node);				\
		return(0);								\
	}

#define DEFINE_LINEAR_REMOVE_NODE(linear_name, list_type, node_type, field)			\
	static inline void linear_name##_remove_node(list_type *list, node_type *node) {	\
		linear_remove_node_v(list->field, field, node);					\
	}

#define DEFINE_LINEAR_DELETE_NODE(linear_name, list_type, node_type, field, release)		\
	static inline void linear_name##_delete_node(list_type *list, node_type *node) {	\
		linear_remove_node_v(list->field, field, node);					\
		release(list, node);								\
		memory_free(node);								\
	}

#define DEFINE_LINEAR_FIND_NODE(linear_name, list_type, node_type, field, compare)	\
	static inline node_type *linear_name##_find_node(list_type *list, void *ptr) {	\
		node_type *cur;								\
											\
		linear_foreach_v(list->field, field, cur) {				\
			if (compare(cur, ptr))						\
				return(cur);						\
		}									\
		return(NULL);								\
	}


#define DEFINE_LINEAR_LIST(linear_name, list_type, node_type, field, release, compare)	\
	DEFINE_LINEAR_INIT_LIST(linear_name, list_type, node_type, field)		\
	DEFINE_LINEAR_RELEASE_LIST(linear_name, list_type, node_type, field, release)	\
	DEFINE_LINEAR_CREATE_LIST(linear_name, list_type, node_type, field)		\
	DEFINE_LINEAR_DESTROY_LIST(linear_name, list_type, node_type, field, release)	\
	DEFINE_LINEAR_CREATE_NODE(linear_name, list_type, node_type, field)		\
	DEFINE_LINEAR_DESTROY_NODE(linear_name, list_type, node_type, field, release)	\
	DEFINE_LINEAR_ADD_NODE(linear_name, list_type, node_type, field)		\
	DEFINE_LINEAR_REMOVE_NODE(linear_name, list_type, node_type, field)		\
	DEFINE_LINEAR_DELETE_NODE(linear_name, list_type, node_type, field, release)	\
	DEFINE_LINEAR_FIND_NODE(linear_name, list_type, node_type, field, compare)


/*** Predefined Generic Linear List Interface ***/

struct linear_node_s {
	linear_node_v(linear_node_s) l;
	void *ptr;
};

struct linear_list_s {
	linear_list_v(linear_node_s) l;
	destroy_t destroy;
};


static inline int linear_init_node(struct linear_list_s *list, struct linear_node_s *node, void *ptr)
{
	node->ptr = ptr;
	return(0);
}

static inline int linear_release_node(struct linear_list_s *list, struct linear_node_s *node)
{
	if (list->destroy)
		list->destroy(node->ptr);
	return(0);
}

#define linear_compare_node(node, ptr)	\
	(node->ptr == ptr)

DEFINE_LINEAR_LIST(linear, struct linear_list_s, struct linear_node_s, l, linear_release_node, linear_compare_node)

static inline struct linear_list_s *create_linear_list(destroy_t destroy)
{
	struct linear_list_s *list;

	if (!(list = linear_create_list()))
		return(NULL);
	list->destroy = destroy;
	return(list);
}

static inline int destroy_linear_list(struct linear_list_s *linear)
{
	linear_destroy_list(linear);
	return(0);
}

#define linear_foreach(list, cur)	\
	linear_foreach_v(list, l, cur)

#define linear_foreach_safe(list, cur, tmp)	\
	linear_foreach_safe_v(list, l, cur, tmp)


#endif

