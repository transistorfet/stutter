/*
 * Name:		hash.h
 * Description:		Hash Table Header
 */

#ifndef _STUTTER_HASH_H
#define _STUTTER_HASH_H

#include <stutter/globals.h>

#define HASH_BF_NO_ADD			0x0001			/** No entries can be added */
#define HASH_BF_NO_REPLACE		0x0002			/** No entries can be replaced */
#define HASH_BF_NO_REMOVE		0x0004			/** No entries can be removed */
#define HASH_BF_CONSTANT		0x0006			/** No entries can be replaced or removed */

struct hash_entry_s {
	char *name;
	void *data;
	struct hash_entry_s *next;
};

struct hash_s {
	int bitflags;
	destroy_t destroy;
	int traverse_index;
	struct hash_entry_s *traverse_next;
	int size;
	int entries;
	struct hash_entry_s **table;
};

struct hash_s *create_hash(short, int, destroy_t);
void destroy_hash(struct hash_s *);

int hash_add(struct hash_s *, const char *, int, void *);
int hash_replace(struct hash_s *, const char *, int, void *);
int hash_remove(struct hash_s *, const char *, int);

struct hash_entry_s *hash_find_entry(struct hash_s *, const char *, int);

void hash_traverse_reset(struct hash_s *);
struct hash_entry_s *hash_traverse_next_entry(struct hash_s *);


static inline void *hash_find(struct hash_s *hash, const char *name, int len) {
	struct hash_entry_s *entry;

	if (!(entry = hash_find_entry(hash, name, len)))
		return(NULL);
	return(entry->data);
}

static inline void *hash_traverse_next(struct hash_s *hash) {
	struct hash_entry_s *entry;

	if (!(entry = hash_traverse_next_entry(hash)))
		return(NULL);
	return(entry->data);
}

#endif

