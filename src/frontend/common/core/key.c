/*
 * Module Name:		key.c
 * Description:		Key Manager
 */

#include <string.h>

#include CONFIG_H
#include <stutter/hash.h>
#include <stutter/macros.h>
#include <stutter/object.h>
#include <stutter/memory.h>
#include <stutter/string.h>
#include <stutter/frontend/common/key.h>

#define FE_KEY_GLOBAL			"global"

#define FE_KEY_INIT_SIZE		32
#define FE_KEY_LOAD_FACTOR		0.75
#define FE_KEY_GROWTH_FACTOR		1.75

#define FE_KEY_MAP(ptr)			( (struct fe_key_map *) (ptr) )
#define FE_KEY_DATA(ptr)		( (union fe_key_data) (ptr) )

#define FE_KEY_BF_SUBMAP		0x01

struct fe_key {
	int ch;
	int bitflags;
	union fe_key_data {
		struct variable_s *var;
		struct fe_key_map *submap;
	} data;
	string_t args;
	struct fe_key *next;
};

struct fe_key_map {
	int size;
	int entries;
	struct fe_key **table;
};

static struct hash_s *context_list;
static struct fe_key_map *global_context = NULL, *current_context = NULL, *current_map = NULL;

static struct fe_key_map *create_fe_key_map(const char *);
void destroy_fe_key_map(struct fe_key_map *);
static void destroy_fe_key(struct fe_key *);
static struct fe_key *fe_key_map_add(struct fe_key_map *, int, int, union fe_key_data, const char *);
static void fe_key_map_remove(struct fe_key_map *, int);
static struct fe_key *fe_key_map_find(struct fe_key_map *, int);
static int fe_key_map_rehash(struct fe_key_map *, int);

int init_key(void)
{
	if (context_list)
		return(0);
	if (!(context_list = create_hash(0, -1, (destroy_t) destroy_fe_key_map)))
		return(-1);
	if (!(global_context = create_fe_key_map(FE_KEY_GLOBAL)))
		return(-1);
	current_context = current_map = global_context;
	return(0);
}

int release_key(void)
{
	if (!context_list)
		return(0);
	destroy_hash(context_list);
	context_list = NULL;
	global_context = NULL;
	current_context = NULL;
	current_map = NULL;
	return(0);
}

/**
 * Associate the given variable and arguments  with the given
 * key sequence under the given context.  If the key cannot be bound,
 * -1 will be returned.  If the key sequence is already bound, the
 * old binding will be destroyed and replace with the given value/type
 * pair and arguments.  A 0 is returned on success.
 */
int fe_key_bind(const char *context, const int *key, struct variable_s *var, const char *args)
{
	int i;
	struct fe_key *cur_key;
	struct fe_key_map *map, *cur_map;

	if ((*key == '\0') || !VARIABLE_GET_TYPE(var)->evaluate)
		return(-1);

	if (!(cur_map = context ? FE_KEY_MAP(hash_find(context_list, context, -1)) : current_context)
	    && !(cur_map = create_fe_key_map(context)))
		return(-1);
	for (i = 0; key[i] != '\0'; i++) {
		cur_key = fe_key_map_find(cur_map, key[i]);
		if (key[i + 1] == '\0') {
			if (!cur_key) {
				/* Create a new terminating key entry */
				if (!(cur_key = fe_key_map_add(cur_map, key[i], 0, FE_KEY_DATA(var), args)))
					return(-1);
				return(0);
			}
			else if (cur_key->bitflags & FE_KEY_BF_SUBMAP) {
				/* Fail (Don't overwrite a submap with a single key) */
				return(-1);
			}
			else {
				/* Overwrite this key with our new key information (Rebind) */
				if (cur_key->args)
					destroy_string(cur_key->args);
				cur_key->bitflags = 0;
				cur_key->data = FE_KEY_DATA(var);
				cur_key->ch = key[i];
				cur_key->args = args ? duplicate_string(args) : NULL;
				return(0);
			}
		}
		else {
			if (!cur_key) {
				/* Create a new submap */
				if (!(map = create_fe_key_map(NULL)))
					return(-1);
				if (!(cur_key = fe_key_map_add(cur_map, key[i], FE_KEY_BF_SUBMAP, FE_KEY_DATA(map), NULL))) {
					destroy_fe_key_map(map);
					return(-1);
				}
			}
			else if (!(cur_key->bitflags & FE_KEY_BF_SUBMAP)) {
				/* Overwrite this key with a new submap */
				if (!(map = create_fe_key_map(NULL)))
					return(-1);
				if (cur_key->args)
					destroy_string(cur_key->args);
				cur_key->data.submap = map;
				cur_key->bitflags = FE_KEY_BF_SUBMAP;
				cur_key->ch = key[i];
				cur_key->args = NULL;
			}
			/* Recurse on the submap */
			cur_map = cur_key->data.submap;
		}
	}
	return(0);
}

/**
 * Find the given key in the given context and remove it from the list
 * of bindings.  If the given key sequence refers to a submap then -1
 * will be returned otherwise 0 is returned.  If the given key is the
 * last key in a given submap then the submap is removed as well.
 */
int fe_key_unbind(const char *context, const int *key)
{
	int i;
	struct fe_key *cur_key;
	struct fe_key_map *cur_map;

	if (!(cur_map = context ? FE_KEY_MAP(hash_find(context_list, context, -1)) : current_context))
		return(-1);
	for (i = 0; key[i] != '\0'; i++) {
		if (key[i + 1] == '\0') {
			/* Remove the entry (whether a submap or terminating key entry) */
			fe_key_map_remove(cur_map, key[i]);
		}
		else if (!(cur_key = fe_key_map_find(cur_map, key[i]))) {
			/* We didn't find the key sequence */
			return(1);
		}
		else if (cur_key->bitflags & FE_KEY_BF_SUBMAP) {
			/* Recurse on the submap */
			cur_map = cur_key->data.submap;
			continue;
		}
		else
			/* We were expecting a submap but found a terminating key entry */
			return(-1);
	}
	return(0);
}

/**
 * Perform a key lookup on one character of a key sequence.  If the
 * key found is a terminal key (not a submap) then the variable's evaluate
 * function for that key is called using the store string specified in the
 * bind_key function; otherwise the submap is made current and is
 * used to look up the key passed during the next call to process_key.
 * If the key is not found, -1 is returned, otherwise 0 is returned.
 */
int fe_key_process(int ch)
{
	string_t args;
	struct fe_key *key;

	if (!current_map && !(current_map = current_context) && !(current_map = global_context))
		return(-1);
	if (!(key = fe_key_map_find(current_map, ch))) {
		if ((current_map != current_context) || (current_context == global_context)) {
			/** If we were processing a multi-key sequence or we are in the global context
			    then we just reset the sequence and return */
			current_map = current_context;
			return(-1);
		}
		else {
			/** Otherwise we check the global context as well */
			if (!(key = fe_key_map_find(global_context, ch)))
				return(-1);
			current_map = global_context;
		}
	}

	if (key->bitflags & FE_KEY_BF_SUBMAP)
		current_map = key->data.submap;
	else {
		if (!(args = duplicate_string(key->args)))
			return(-1);
        	VARIABLE_GET_TYPE(key->data.var)->evaluate(key->data.var, args);
		destroy_string(args);
		current_map = current_context;
	}
	return(0);
}

/**
 * Select the given context to be used when processing key sequences.
 * If the given context does not exist, a new context with the given
 * name will be created and made current.  A 0 is returned on success
 * or -1 on failure.  If a failure occurs, the context will not change.
 */
int fe_key_select_context(const char *context)
{
	struct fe_key_map *map;

	if (!context)
		context = FE_KEY_GLOBAL;
	if (!(map = hash_find(context_list, context, -1)) && !(map = create_fe_key_map(context)))
		return(-1);
	current_context = global_context;
	return(0);
}

/*** Local Functions ***/

static struct fe_key_map *create_fe_key_map(const char *context)
{
	struct fe_key_map *map;

	if (!(map = (struct fe_key_map *) memory_alloc(sizeof(struct fe_key_map))))
		return(NULL);
	if (!(map->table = (struct fe_key **) memory_alloc(FE_KEY_INIT_SIZE * sizeof(struct fe_key *)))) {
		memory_free(map);
		return(NULL);
	}
	map->size = FE_KEY_INIT_SIZE;
	memset(map->table, '\0', map->size * sizeof(struct fe_key *));
	hash_add(context_list, context, -1, map);
	return(map);
}

void destroy_fe_key_map(struct fe_key_map *map)
{
	int i;
	struct fe_key *cur, *tmp;

	if (map->table) {
		for (i = 0; i < map->size; i++) {
			for (cur = map->table[i]; cur; cur = tmp) {
				tmp = cur->next;
				destroy_fe_key(cur);
			}
		}
		memory_free(map->table);
	}
}

static void destroy_fe_key(struct fe_key *key)
{
	// TODO should you destroy the variable too?  (You'd need reference counting i think)
	if (key->bitflags & FE_KEY_BF_SUBMAP)
		destroy_fe_key_map(key->data.submap);
	if (key->args)
		destroy_string(key->args);
	memory_free(key);
}


static struct fe_key *fe_key_map_add(struct fe_key_map *map, int ch, int bitflags, union fe_key_data data, const char *args)
{
	struct fe_key *key;

	if (!(key = (struct fe_key *) memory_alloc(sizeof(struct fe_key))))
		return(NULL);
	key->ch = ch;
	key->bitflags = bitflags;
	key->data = data;
	key->args = args ? duplicate_string(args) : NULL;

	key->next = map->table[ch % map->size];
	map->table[ch % map->size] = key;
	map->entries++;
	if ((map->entries / map->size) > FE_KEY_LOAD_FACTOR)
		fe_key_map_rehash(map, map->size * FE_KEY_GROWTH_FACTOR);
	return(key);
}

static void fe_key_map_remove(struct fe_key_map *map, int ch)
{
	struct fe_key *cur, *prev;

	for (prev = NULL, cur = map->table[ch % map->size]; cur; prev = cur, cur = cur->next) {
		if (cur->ch == ch) {
			if (prev)
				prev->next = cur->next;
			else
				map->table[ch % map->size] = cur->next;
			destroy_fe_key(cur);
			return;
		}
	}
}

static struct fe_key *fe_key_map_find(struct fe_key_map *map, int ch)
{
	struct fe_key *cur;

	for (cur = map->table[ch % map->size]; cur; cur = cur->next) {
		if (cur->ch == ch)
			return(cur);
	}
	return(NULL);
}

static int fe_key_map_rehash(struct fe_key_map *map, int newsize)
{
	// TODO implement
	return(0);
}



