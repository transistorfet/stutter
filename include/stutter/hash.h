/*
 * Virtual Module Name:	hash.h
 * Version:		0.1
 * Description:		Hash Table Virtual Interface
 */

#ifndef _STUTTER_HASH_H
#define _STUTTER_HASH_H

#include <stutter/memory.h>
#include <stutter/globals.h>

typedef unsigned int (*hash_t)(void *);

/*** Virtual Interface ***/

#define hash_node_v(node_type)					\
	struct {						\
		struct node_type *next;				\
	}

#define hash_list_v(node_type)					\
	struct {						\
		int size;					\
		int entries;					\
		struct node_type **table;			\
	}

#define hash_init_v(list, init_size) {									\
	(list).size = (init_size);										\
	(list).entries = 0;										\
	if ((list).table = (typeof((list).table)) memory_alloc((init_size) * sizeof(typeof((list).table[0]))))	\
		memset((list).table, '\0', (init_size) * sizeof(typeof((list).table[0])));				\
}

#define hash_release_v(list) {					\
	if ((list).table) {					\
		memory_free((list).table);			\
		(list).table = NULL;				\
	}							\
}

#define hash_add_node_v(list, field, node, hashnum) {		\
	unsigned int hash;					\
	hash = hashnum;						\
	(node)->field.next = (list).table[(hash)];		\
	(list).table[(hash)] = (node);				\
	(list).entries++;					\
}

#define hash_remove_node_v(list, field, ptr, hashnum, compare) {	\
	unsigned int hash;						\
	typeof((list).table[0]) cur;					\
	typeof((list).table[0]) prev;					\
	hash = hashnum;							\
	ptr = NULL;							\
	prev = NULL;							\
	cur = (list).table[(hash)];					\
	while (cur) {							\
		if (compare) {						\
			if (prev)					\
				prev->field.next = cur->field.next;	\
			else						\
				(list).table[(hash)] = cur->field.next;	\
			ptr = cur;					\
			(list).entries--;				\
			break;						\
		}							\
		prev = cur;						\
		cur = cur->field.next;					\
	}								\
}

#define hash_find_node_v(list, field, ptr, hashnum, compare) {		\
	unsigned int hash;						\
	typeof((list).table[0]) cur;					\
	hash = hashnum;							\
	ptr = NULL;							\
	cur = (list).table[(hash)];					\
	while (cur) {							\
		if (compare) {						\
			ptr = cur;					\
			break;						\
		}							\
		cur = cur->field.next;					\
	}								\
}

#define hash_rehash_v(list, field, newsize, hash_func, failed_body) {					\
	int i, hash, oldsize;										\
	typeof((list).table) newtable;									\
	typeof((list).table[0]) cur;									\
	typeof((list).table[0]) tmp;									\
													\
	if (newtable = (typeof(newtable)) memory_alloc((newsize) * sizeof(typeof(newtable[0])))) {	\
		memset(newtable, '\0', (newsize) * sizeof(typeof(newtable[0])));			\
		oldsize = (list).size;									\
		(list).size = newsize;									\
		for (i = 0;i < oldsize;i++) {								\
			cur = (list).table[i];								\
			while (cur) {									\
				tmp = cur->field.next;							\
				hash = hash_func;							\
				cur->field.next = newtable[hash];					\
				newtable[hash] = cur;							\
				cur = tmp;								\
			}										\
		}											\
		memory_free((list).table);								\
		(list).table = newtable;								\
	}												\
	else {												\
		failed_body;										\
	}												\
}

#define hash_foreach_v(list, field, inc, cur)				\
	for (inc = 0;inc < (list).size;inc++)				\
		for (cur = (list).table[inc];cur;cur = cur->field.next)

#define hash_foreach_safe_v(list, field, inc, cur, tmp)			\
	for (inc = 0;inc < (list).size;inc++)				\
		for (cur = (list).table[inc];cur && ((tmp = cur->field.next), 1);cur = tmp)

#define hash_next_v(field, node)	( (node)->field.next )

#define hash_size_v(list)		(list).size
#define hash_entries_v(list)		(list).entries
#define hash_load_v(list)		( ((float) hash_entries_v(list)) / ((float) hash_size_v(list)) )
#define hash_is_initialized_v(list)	( (list).table )

/*** Hash Functions ***/

static inline unsigned int sdbm_partial_hash(char *str, int len) {
	int i;
	unsigned int hash = 0;

	for (i = 0;len && (i < len) && (str[i] != '\0');i++)
		hash = str[i] + (hash << 6) + (hash << 16) - hash;
	return(hash);
}

static inline unsigned int sdbm_hash(char *str) {
	int i;
	unsigned int hash = 0;

	for (i = 0;str[i] != '\0';i++)
		hash = str[i] + (hash << 6) + (hash << 16) - hash;
	return(hash);
}

static inline unsigned int sdbm_partial_hash_icase(char *str, int len) {
	int i;
	unsigned int hash = 0;

	for (i = 0;len && (i < len) && (str[i] != '\0');i++)
		hash = (((str[i] >= 'A') && (str[i] <= 'Z')) ? str[i] + 0x20 : str[i]) + (hash << 6) + (hash << 16) - hash;
	return(hash);
}

static inline unsigned int sdbm_hash_icase(char *str) {
	int i;
	unsigned int hash = 0;

	for (i = 0;str[i] != '\0';i++)
		hash = (((str[i] >= 'A') && (str[i] <= 'Z')) ? str[i] + 0x20 : str[i]) + (hash << 6) + (hash << 16) - hash;
	return(hash);
}

/*** Inline Interface ***/

#define DEFINE_HASH_INIT_TABLE(hash_name, list_type, node_type, field)			\
	static inline int hash_name##_init_table(list_type *table, int init_size) {	\
		hash_init_v(table->field, init_size);					\
		return(0);								\
	}

#define DEFINE_HASH_RELEASE_TABLE(hash_name, list_type, node_type, field, release)	\
	static inline void hash_name##_release_table(list_type *table) {		\
		int i;									\
		node_type *cur, *tmp;							\
											\
		hash_foreach_safe_v(table->field, field, i, cur, tmp) {			\
			release(table, cur);						\
			memory_free(cur);						\
		}									\
		hash_release_v(table->field);						\
	}

#define DEFINE_HASH_CREATE_TABLE(hash_name, list_type, node_type, field)	\
	static inline list_type *hash_name##_create_table(int init_size) {	\
		list_type *table;						\
										\
		if (!(table = (list_type *) memory_alloc(sizeof(list_type))))	\
			return(NULL);						\
		hash_init_v(table->field, init_size);				\
		return(table);							\
	}

#define DEFINE_HASH_DESTROY_TABLE(hash_name, list_type, node_type, field, release)	\
	static inline int hash_name##_destroy_table(list_type *table) {		\
		int i;									\
		node_type *cur, *tmp;							\
											\
		hash_foreach_safe_v(table->field, field, i, cur, tmp) {			\
			release(table, cur);						\
			memory_free(cur);						\
		}									\
		hash_release_v(table->field);						\
		memory_free(table);							\
		return(0);								\
	}

#define DEFINE_HASH_CREATE_NODE(hash_name, list_type, node_type, field)				\
	static inline node_type *hash_name##_create_node(int size) {				\
		node_type *node;								\
												\
		if (!(node = (node_type *) memory_alloc(size ? size : sizeof(node_type))))	\
			return(NULL);								\
		return(node);									\
	}

#define DEFINE_HASH_DESTROY_NODE(hash_name, list_type, node_type, field, release)		\
	static inline int hash_name##_destroy_node(list_type *table, node_type *node) {		\
		release(table, node);								\
		memory_free(node);								\
		return(0);									\
	}

#define DEFINE_HASH_ADD_NODE(hash_name, list_type, node_type, field, keyfield, hashfunc, compare, hashload, hashgrowth)		\
	static inline int hash_name##_add_node(list_type *table, node_type *node) {					\
		hash_add_node_v(table->field, field, node, hashfunc(table, node->keyfield) % hash_size_v(table->field));			\
		if (hash_load_v(table->field) > hashload)								\
			hash_rehash_v(table->field, field, (hash_size_v(table->field) * hashgrowth), hashfunc(table, cur->keyfield) % hash_size_v(table->field), return(-1));	\
		return(0);												\
	}

#define DEFINE_HASH_REMOVE_NODE(hash_name, list_type, node_type, field, keyfield, hashfunc, compare)	\
	static inline node_type *hash_name##_remove_node(list_type *table, void *key) {			\
		node_type *node;									\
													\
		hash_remove_node_v(table->field, field, node, hashfunc(table, key) % hash_size_v(table->field), compare(cur, key));	\
		return(node);										\
	}

#define DEFINE_HASH_DELETE_NODE(hash_name, list_type, node_type, field, keyfield, release, hashfunc, compare)	\
	static inline int hash_name##_delete_node(list_type *table, void *key) {				\
		node_type *node;										\
														\
		hash_remove_node_v(table->field, field, node, hashfunc(table, key) % hash_size_v(table->field), compare(cur, key));	\
		if (!node)											\
			return(-1);										\
		release(table, node);										\
		memory_free(node);										\
		return(0);											\
	}

#define DEFINE_HASH_FIND_NODE(hash_name, list_type, node_type, field, keyfield, hashfunc, compare)	\
	static inline node_type *hash_name##_find_node(list_type *table, void *key) {			\
		node_type *node;									\
													\
		hash_find_node_v(table->field, field, node, hashfunc(table, key) % hash_size_v(table->field), compare(cur, key));	\
		return(node);										\
	}

#define DEFINE_HASH_NEXT_NODE(hash_name, list_type, node_type, field)		\
	static inline node_type *hash_name##_next_node(node_type *node) {	\
		return(node ? hash_next_v(field, node) : NULL);			\
	}

#define DEFINE_HASH_TABLE(hash_name, list_type, node_type, field, keyfield, release, hashfunc, compare, hashload, hashgrowth)	\
	DEFINE_HASH_INIT_TABLE(hash_name, list_type, node_type, field)								\
	DEFINE_HASH_RELEASE_TABLE(hash_name, list_type, node_type, field, release)						\
	DEFINE_HASH_CREATE_TABLE(hash_name, list_type, node_type, field)							\
	DEFINE_HASH_DESTROY_TABLE(hash_name, list_type, node_type, field, release)						\
	DEFINE_HASH_CREATE_NODE(hash_name, list_type, node_type, field)								\
	DEFINE_HASH_DESTROY_NODE(hash_name, list_type, node_type, field, release)						\
	DEFINE_HASH_ADD_NODE(hash_name, list_type, node_type, field, keyfield, hashfunc, compare, hashload, hashgrowth)		\
	DEFINE_HASH_REMOVE_NODE(hash_name, list_type, node_type, field, keyfield, hashfunc, compare)				\
	DEFINE_HASH_DELETE_NODE(hash_name, list_type, node_type, field, keyfield, release, hashfunc, compare)			\
	DEFINE_HASH_FIND_NODE(hash_name, list_type, node_type, field, keyfield, hashfunc, compare)				\
	DEFINE_HASH_NEXT_NODE(hash_name, list_type, node_type, field)


/*** Predefined Generic String-Indexed Hash Table ***/

#ifndef HASH_LOAD_FACTOR
#define HASH_LOAD_FACTOR	0.75
#endif

#ifndef HASH_GROWTH_FACTOR
#define HASH_GROWTH_FACTOR	1.75
#endif

struct hash_node_s {
	hash_node_v(hash_node_s) h;
	char *key;
	void *ptr;
};

struct hash_table_s {
	hash_list_v(hash_node_s) h;
	destroy_t destroy;
};

static inline int hash_init_node(struct hash_table_s *table, struct hash_node_s *node, char *key, void *ptr)
{
	node->key = key;
	node->ptr = ptr;
	return(0);
}

static inline int hash_release_node(struct hash_table_s *table, struct hash_node_s *node)
{
	if (table->destroy)
		table->destroy(node->ptr);
	return(0);
}

#define hash_hashfunc(list, key)	\
	sdbm_hash(key)

#define hash_compare_node(node, ptr)	\
	(!strcmp(node->key, ptr))

DEFINE_HASH_TABLE(hash, struct hash_table_s, struct hash_node_s, h, key, hash_release_node, hash_hashfunc, hash_compare_node, HASH_LOAD_FACTOR, HASH_GROWTH_FACTOR)

static inline struct hash_table_s *create_hash_table(int init_size, destroy_t destroy)
{
	struct hash_table_s *table;

	if (!(table = hash_create_table(init_size)))
		return(NULL);
	table->destroy = destroy;
	return(table);
}

static inline int destroy_hash_table(struct hash_table_s *table)
{
	return(hash_destroy_table(table));
}

#define hash_foreach(table, inc, cur)	\
	hash_foreach_v(table, h, inc, cur)

#define hash_foreach_safe(table, inc, cur, tmp)	\
	hash_foreach_safe_v(table, h, inc, cur, tmp)

#endif

