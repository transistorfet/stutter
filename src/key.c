/*
 * Module Name:		key.c
 * Version:		0.1
 * Module Requirements:	memory ; string ; callback
 * Description:		Key Manager
 */

#include <key.h>
#include <nit/list.h>
#include <nit/memory.h>
#include <nit/string.h>
#include <nit/callback.h>

#define KEY_INITIAL_BASE_ROOT	2 //50
#define KEY_INITIAL_ROOT	20
#define KEY_INITIAL_SUBMAP	10
#define KEY_LOAD_FACTOR		0.75

#define KEY_KBF_SUBMAP		0x01

#define key_hash(list, key)	( key % list->size )

struct key_s {
	short ch;
	short bitflags;
	union key_data_u {
		struct callback_s *callback;
		struct key_map_s *submap;
	} data;
	string_t args;
	struct key_s *next;
};

struct key_map_s {
	string_t context;
	int size;
	int entries;
	struct key_s **table;
};

static struct list_s *context_list;
static struct key_map_s *current_root, *current_map;

static struct key_s *create_key(short, short, union key_data_u, string_t);
static void destroy_key(struct key_s *);
static struct key_map_s *create_key_map(char *, int);
static int compare_key_map_context(struct key_map_s *, char *);
static void destroy_key_map(struct key_map_s *);
static int rehash_key_map(struct key_map_s *, int);

int init_key(void)
{
	struct key_map_s *root;

	if (context_list)
		return(1);
	if (!(context_list = create_list(0, (compare_t) compare_key_map_context, (destroy_t) destroy_key_map)))
		return(-1);
	if (!(root = create_key_map("", KEY_INITIAL_BASE_ROOT))) {
		destroy_list(context_list);
		context_list = NULL;
		return(-1);
	}
	list_add(context_list, root);
	current_root = root;
	current_map = root;
	return(0);
}

int release_key(void)
{
	destroy_list(context_list);
	return(0);
}

/**
 * Associate the given function and pointer callback with the given
 * key sequence under the given context.  If the key cannot be bound,
 * -1 will be returned.  If the key sequence is already bound, the
 * old binding will be destroyed and replace with the given callback.
 * A 0 is returned on success.
 */
int bind_key(char *context, char *str, struct callback_s *callback, string_t args)
{
	int i, hash;
	struct key_s *key, *cur_key;
	struct key_map_s *map, *cur_map;

	// TODO is the given key sequence the acutal sequence or a human-writable representation?

	if (*str == '\0')
		return(-1);
	if (!(cur_map = context ? list_find(context_list, context, 0) : current_root)) {
		if (!(cur_map = create_key_map(context, KEY_INITIAL_ROOT)))
			return(-1);
		list_add(context_list, cur_map);
	}
	for (i = 0;str[i] != '\0';i++) {
		hash = key_hash(cur_map, str[i]);
		cur_key = cur_map->table[hash];
		while (cur_key) {
			if (cur_key->ch == str[i])
				break;
			cur_key = cur_key->next;
		}
		if (cur_key) {
			if (cur_key->bitflags & KEY_KBF_SUBMAP) {
				/** If the key already exists as a submap and we are at the
				    terminating character of the sequence then we cannot map
				    this key and fail; otherwise we use this keymap and avoid
				    creating a new one */
				if (str[i + 1] == '\0')
					return(-1);
				cur_map = cur_key->data.submap;
				continue;
			}
			else
				return(-1);
		}
		else {
			if (str[i + 1] == '\0') {
				if (!(key = create_key(str[i], 0, (union key_data_u) callback, args)))
					return(-1);
			}
			else {
				if (!(map = create_key_map(NULL, KEY_INITIAL_SUBMAP)))
					return(-1);
				if (!(key = create_key(str[i], KEY_KBF_SUBMAP, (union key_data_u) map, NULL))) {
					destroy_key_map(map);
					return(-1);
				}
			}
			key->next = cur_map->table[hash];
			cur_map->table[hash] = key;
			cur_map->entries++;
			if ((cur_map->entries / cur_map->size) > KEY_LOAD_FACTOR)
				rehash_key_map(cur_map, cur_map->entries + (cur_map->entries * 0.75));
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
int unbind_key(char *context, char *str)
{
	int i, hash;
	struct key_s *cur_key, *prev_key;
	struct key_map_s *cur_map;

	if (!(cur_map = context ? list_find(context_list, context, 0) : current_root))
		return(-1);
	for (i = 0;str[i] != '\0';i++) {
		hash = key_hash(cur_map, str[i]);
		prev_key = NULL;
		cur_key = cur_map->table[hash];
		while (cur_key) {
			if (cur_key->ch == str[i]) {
				if (cur_key->bitflags & KEY_KBF_SUBMAP) {
					cur_map = cur_key->data.submap;
					break;
				}
				else if (str[i + 1] == '\0') {
					if (prev_key)
						prev_key->next = cur_key->next;
					else
						cur_map->table[hash] = cur_key->next;
					cur_map->entries--;
					destroy_key(cur_key);
				}
				else
					return(-1);
			}
			prev_key = cur_key;
			cur_key = cur_key->next;
		}
		if (!cur_key)
			return(-1);
	}
	return(-1);
}

/**
 * Perform a key lookup on one character of a key sequence.  If the
 * key found is a terminal key (not a submap) then the callback for
 * that key is called using the store string specified in the
 * bind_key function; otherwise the submap is made current and is
 * used to look up the key passed during the next call to process_key.
 * If the key is not found, -1 is returned, otherwise 0 is returned.
 */
int process_key(int ch)
{
	struct key_s *cur;

	cur = current_map->table[key_hash(current_map, ch)];
	while (cur) {
		if (cur->ch == ch) {
			if (cur->bitflags & KEY_KBF_SUBMAP)
				current_map = cur->data.submap;
			else {
				execute_callback(cur->data.callback, cur->args);
				current_map = current_root;
			}
			return(0);
		}
		cur = cur->next;
	}
	return(-1);
}

/**
 * Select the given context to be used when processing key sequences.
 * If the given context does not exist, a new context with the given
 * name will be created and made current.  A 0 is returned on success
 * or -1 on failure.  If a failure occurs, the context will not change.
 */
int select_key_context(char *context)
{
	struct key_map_s *root;

	if (!(root = list_find(context_list, context, 0))) {
		if (!(root = create_key_map(context, KEY_INITIAL_ROOT)))
			return(-1);
		list_add(context_list, root);
	}
	current_root = root;
	return(0);
}

/*** Local Functions ***/

/**
 * Create a new key struct with all the given parameters.
 */
static struct key_s *create_key(short ch, short bitflags, union key_data_u data, string_t args)
{
	struct key_s *key;

	if (!(key = (struct key_s *) memory_alloc(sizeof(struct key_s))))
		return(NULL);
	key->ch = ch;
	key->bitflags = bitflags;
	key->data = data;
	key->args = args;
	key->next = NULL;

	return(key);
}

/**
 * Free the resources used by the given key struct.
 */
static void destroy_key(struct key_s *key)
{
	if (key) {
		if (key->bitflags & KEY_KBF_SUBMAP)
			destroy_key_map(key->data.submap);
		else
			destroy_callback(key->data.callback);
		if (key->args)
			destroy_string(key->args);
		memory_free(key);
	}
}

/**
 * Create a new key map of the given initial size and return a
 * pointer to it or return NULL on error.
 */
static struct key_map_s *create_key_map(char *context, int size)
{
	struct key_map_s *map;

	if (!(map = (struct key_map_s *) memory_alloc(sizeof(struct key_map_s))))
		return(NULL);
	if (!(map->table = (struct key_s **) memory_alloc(size * sizeof(struct key_s *)))) {
		memory_free(map);
		return(NULL);
	}
	map->context = context ? create_string(context) : NULL;
	map->size = size;
	map->entries = 0;
	return(map);
}

/**
 * Compare the given context name and the name of the context in the
 * given map and if they are equal then return 0 otherwise return non-zero.
 */
static int compare_key_map_context(struct key_map_s *map, char *context)
{
	if (map->context && context)
		return(strcmp(map->context, context));
	return(-1);
}

/**
 * Free the resources used by the given map and all of its submaps
 * and key bindings.  A 0 is returned on success or -1 on failure.
 */
static void destroy_key_map(struct key_map_s *map)
{
	int i;
	struct key_s *cur, *tmp;

	for (i = 0;i < map->size;i++) {
		cur = map->table[i];
		while (cur) {
			tmp = cur->next;
			destroy_key(cur);
			cur = tmp;
		}
	}
}

/**
 * Resize the given key map to the given newsize and rehash the
 * elements into it the new hashtable.
 */
static int rehash_key_map(struct key_map_s *map, int newsize)
{
	int i, hash, oldsize;
	struct key_s **table, *cur, *tmp;

	if (!(table = (struct key_s **) memory_alloc(newsize * sizeof(struct key_s *))))
		return(-1);
	oldsize = map->size;
	map->size = newsize;
	for (i = 0;i < oldsize;i++) {
		cur = map->table[i];
		while (cur) {
			tmp = cur->next;
			hash = key_hash(map, cur->ch);
			cur->next = table[hash];
			table[hash] = cur;
			cur = tmp;
		}
	}
	memory_free(map->table);
	map->table = table;
	return(0);
}

