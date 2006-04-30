/*
 * Virtual Module Name:	hash.h
 * Version:		0.1
 * Description:		Hash Table Virtual Interface
 */

#ifndef _STUTTER_LIB_HASH_H
#define _STUTTER_LIB_HASH_H

#include <stutter/lib/memory.h>
#include <stutter/lib/globals.h>

typedef unsigned int (*hash_t)(void *);

/*** Virtual Interface ***/

#ifndef HASH_LOAD_FACTOR
#define HASH_LOAD_FACTOR	0.75
#endif

#ifndef HASH_GROW_FACTOR
#define HASH_GROW_FACTOR	1.75
#endif

#ifndef HASH_INITIAL_SIZE
#define HASH_INITIAL_SIZE	10
#endif

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

#define hash_rehash_v(list, field, newsize, hash_func) {						\
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
				cur->field.next = newtable[hash];						\
				newtable[hash] = cur;							\
				cur = tmp;								\
			}										\
		}											\
		memory_free((list).table);								\
		(list).table = newtable;									\
	}												\
}

#define hash_destroy_list_v(list, field, body) {	\
	int i;						\
	typeof((list).table[0]) cur;			\
	typeof((list).table[0]) tmp;			\
							\
	for (i = 0;i < (list).size;i++) {		\
		cur = (list).table[i];			\
		while (cur) {				\
			tmp = cur->field.next;		\
			body;				\
			cur = tmp;			\
		}					\
	}						\
	/*memory_free((list).table);*/			\
}

#define hash_size_v(list)		(list).size
#define hash_entries_v(list)		(list).entries
#define hash_load_v(list)		( hash_entries_v(list) / hash_size_v(list) )

#define hash_add_node_and_grow_v(list, field, node, name, hashfunc) {	\
	hash_add_node_v(list, field, node, hashfunc(list, name));	\
	if (hash_load_v(list) > HASH_LOAD_FACTOR)			\
		hash_rehash_v(list, field, (hash_size_v(list) * HASH_GROW_FACTOR), hashfunc(list, node->name));		\
}

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

/*** Inline Interface ***/

struct hash_node_s {
	void *ptr;
	hash_node_v(hash_node_s) list;
};

struct hash_table_s {
	hash_t hash;
	compare_t compare;
	hash_list_v(hash_node_s) list;
};

static inline struct hash_table_s *hash_create_list(int init_size, hash_t hash, compare_t compare)
{
	struct hash_table_s *table;

	if (!(table = (struct hash_table_s *) memory_alloc(sizeof(struct hash_table_s))))
		return(NULL);
	init_size = init_size ? init_size : HASH_INITIAL_SIZE;
	hash_init_v(table->list, init_size);
	table->hash = hash;
	table->compare = compare;
	return(table);
}

static inline void hash_destroy_list(struct hash_table_s *table, destroy_t destroy)
{
	hash_destroy_list_v(table->list, list,
		destroy(cur->ptr);
		memory_free(cur);
	);
	hash_release_v(table->list);
	memory_free(table);
}

static inline int hash_add_node(struct hash_table_s *table, void *key, void *ptr)
{
	struct hash_node_s *node;

	if (!(node = (struct hash_node_s *) memory_alloc(sizeof(struct hash_node_s))))
		return(-1);
	node->ptr = ptr;
	hash_add_node_v(table->list, list, node, table->hash(key) % hash_size_v(table->list));
	if (hash_load_v(table->list) > HASH_LOAD_FACTOR)
		hash_rehash_v(table->list, list, (hash_size_v(table->list) * 1.75), table->hash(key) % hash_size_v(table->list));
	return(0);
}

static inline void *hash_remove_node(struct hash_table_s *table, void *key)
{
	void *ptr;
	struct hash_node_s *node;

	hash_remove_node_v(table->list, list, node, table->hash(key) % hash_size_v(table->list), table->compare(cur->ptr, key));
	if (!node)
		return(NULL);
	ptr = node->ptr;
	memory_free(node);
	return(ptr);
}

static inline void *hash_find_node(struct hash_table_s *table, void *key)
{
	struct hash_node_s *node;

	hash_find_node_v(table->list, list, node, table->hash(key) % hash_size_v(table->list), table->compare(cur->ptr, key));
	if (!node)
		return(NULL);
	return(node->ptr);
}


#endif

