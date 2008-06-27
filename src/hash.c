/*
 * Name:		hash.c
 * Description:		Hash Table
 */


#include <stdlib.h>
#include <string.h>

#include <stutter/hash.h>
#include <stutter/memory.h>
#include <stutter/globals.h>

#define HASH_INIT_SIZE			32
#define HASH_LOAD_FACTOR		0.75
#define HASH_GROWTH_FACTOR		1.75

/** If the len is negative then we ignore len.  We assume the name is null-terminated
    so we make len big and the null char will be caught first. */
#define HASH_STRING_MAX			65536

#define LOWERCASE(ch) \
	( (ch >= 0x41 && ch <= 0x5a) ? ch + 0x20 : ch )

static inline int hash_entries_rehash(struct hash_s *, int);
static inline unsigned int hash_func(const char *, int);

/**
 * Allocate a hash table generic data.
 */
struct hash_s *create_hash(short bitflags, int init_size, destroy_t destroy)
{
	struct hash_entry_s **table;
	struct hash_s *env;

	if (init_size <= 0)
		init_size = HASH_INIT_SIZE;
	if (!(table = (struct hash_entry_s **) memory_alloc(init_size * sizeof(struct hash_entry_s *))))
		return(NULL);
	if (!(env = (struct hash_s *) memory_alloc(sizeof(struct hash_s)))) {
		memory_free(table);
		return(NULL);
	}
	env->bitflags = bitflags;
	env->destroy = destroy;
	env->traverse_index = 0;
	env->traverse_next = NULL;
	env->size = init_size;
	env->entries = 0;
	env->table = table;
	memset(env->table, '\0', env->size * sizeof(struct hash_entry_s *));
	return(env);
}

/**
 * Free resources allocated by the hash table.
 */
void destroy_hash(struct hash_s *env)
{
	unsigned int i;
	struct hash_entry_s *cur, *next;

	for (i = 0;i < env->size;i++) {
		cur = env->table[i];
		while (cur) {
			next = cur->next;
			if (env->destroy)
				env->destroy(cur->data);
			memory_free(cur);
			cur = next;
		}
	}
	memory_free(env->table);
	memory_free(env);
}


/**
 * Add an entry of "name" to the hash table.
 */
int hash_add(struct hash_s *env, const char *name, int len, void *data)
{
	unsigned int hash;
	struct hash_entry_s *entry;

	if (!name || !data || (env->bitflags & HASH_BF_NO_ADD))
		return(-1);
	/** If the len is negative then ignore len.  We assume the name is
	    null-terminated so we make len big and the null char will be
	    caught first. */
	if (len < 0)
		len = HASH_STRING_MAX;
	/** Search for an existing entry */
	hash = hash_func(name, len) % env->size;
	for (entry = env->table[hash]; entry; entry = entry->next) {
		if (!strncasecmp(name, entry->name, len))
			return(-1);
	}

	if (!(entry = (struct hash_entry_s *) memory_alloc(sizeof(struct hash_entry_s) + strlen(name) + 1)))
		return(-1);
	entry->name = (char *) (entry + 1);
	strcpy(entry->name, name);
	entry->data = data;

	entry->next = env->table[hash];
	env->table[hash] = entry;
	env->entries++;
	if ((env->entries / env->size) > HASH_LOAD_FACTOR)
		hash_entries_rehash(env, env->size * HASH_GROWTH_FACTOR);
	return(0);
}

/**
 * Replace the entry's data with "data".
 */
int hash_replace(struct hash_s *env, const char *name, int len, void *data)
{
	unsigned int hash;
	struct hash_entry_s *cur;

	if (!name || !data || (env->bitflags & HASH_BF_NO_REPLACE))
		return(-1);
	/** If the len is negative then ignore len.  We assume the name is
	    null-terminated so we make len big and the null char will be
	    caught first. */
	if (len < 0)
		len = HASH_STRING_MAX;
	hash = hash_func(name, len) % env->size;
	for (cur = env->table[hash % env->size]; cur; cur = cur->next) {
		if (!strncasecmp(name, cur->name, len)) {
			if (env->destroy)
				env->destroy(cur->data);
			cur->data = data;
			return(0);
		}
	}
	return(-1);
}

/**
 * Remove an entry of "name" from hash table
 */
int hash_remove(struct hash_s *env, const char *name, int len)
{
	unsigned int hash;
	struct hash_entry_s *cur, *prev;

	if (!name || (env->bitflags & HASH_BF_NO_REMOVE))
		return(-1);
	/** If the len is negative then ignore len.  We assume the name is
	    null-terminated so we make len big and the null char will be
	    caught first. */
	if (len < 0)
		len = HASH_STRING_MAX;
	hash = hash_func(name, len) % env->size;
	prev = NULL;
	cur = env->table[hash];
	while (cur) {
		if (!strncasecmp(name, cur->name, len)) {
			if (env->traverse_next == cur)
				hash_traverse_next(env);
			if (prev)
				prev = cur->next;
			else
				env->table[hash] = cur->next;
			if (env->destroy)
				env->destroy(cur->data);
			memory_free(cur);
			env->entries--;
			return(0);
		}
		prev = cur;
		cur = cur->next;
	}
	return(-1);
}

/**
 * Find the value bound to name in table.
 */
struct hash_entry_s *hash_find_entry(struct hash_s *env, const char *name, int len)
{
	unsigned int hash;
	struct hash_entry_s *cur;

	if (len < 0)
		len = HASH_STRING_MAX;
	hash = hash_func(name, len) % env->size;
	for (cur = env->table[hash % env->size]; cur; cur = cur->next) {
		if (!strncasecmp(name, cur->name, len))
			return(cur);
	}
	return(NULL);
}

void hash_traverse_reset(struct hash_s *env)
{
	for (env->traverse_index = 0; env->traverse_index < env->size; env->traverse_index++) {
		if ((env->traverse_next = env->table[env->traverse_index]))
			return;
	}
}

struct hash_entry_s *hash_traverse_next_entry(struct hash_s *env)
{
	struct hash_entry_s *entry;

	if ((env->traverse_index >= env->size) || !(entry = env->traverse_next))
		return(NULL);
	else if ((env->traverse_next = env->traverse_next->next))
		return(entry);

	while (++env->traverse_index < env->size) {
		if ((env->traverse_next = env->table[env->traverse_index]))
			return(entry);
	}
	return(entry);
}


/*** Local Functions ***/

/**
 * Increases the size of the hashtable and reinserts all of the elements.
 */
static inline int hash_entries_rehash(struct hash_s *env, int newsize)
{
	unsigned int i, hash, oldsize;
	struct hash_entry_s **newtable;
	struct hash_entry_s *cur, *next;

	if (!(newtable = (struct hash_entry_s **) memory_alloc(newsize * sizeof(struct hash_entry_s *))))
		return(-1);
	memset(newtable, '\0', newsize * sizeof(struct hash_entry_s *));
	oldsize = env->size;
	env->size = newsize;
	for (i = 0;i < oldsize;i++) {
		cur = env->table[i];
		while (cur) {
			next = cur->next;
			hash = hash_func(cur->name, HASH_STRING_MAX) % env->size;
			cur->next = newtable[hash];
			newtable[hash] = cur;
			cur = next;
		}
	}
	memory_free(env->table);
	env->table = newtable;
	return(0);
}

static inline unsigned int hash_func(const char *str, int len)
{
	int i;
	unsigned int hash = 0;

	for (i = 0;i < len && str[i] != '\0';i++)
		hash = LOWERCASE(str[i]) + (hash << 6) + (hash << 16) - hash;
	return(hash);
}

