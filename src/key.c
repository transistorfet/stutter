/*
 * Module Name:		key.c
 * Version:		0.1
 * Module Requirements:	memory ; string ; callback
 * Description:		Key Manager
 */

#include <key.h>
#include <nit/memory.h>
#include <nit/string.h>
#include <nit/callback.h>

#define KEY_INITIAL_ROOT	50
#define KEY_INITIAL_SUBMAP	10

#define MAX_KEYS		256
#define MAX_BUFFER		20

#define KEY_KBF_SUBMAP		0x01

#define key_hash(list, key)	( key % list->size )

typedef char *context_t;

struct key_s {
	short ch;
	short bitflags;
	context_t context;
	union key_data_u {
		struct callback_s *callback;
		struct key_map_s *submap;
	} data;
	string_t args;
	struct key_s *next;
};

struct key_map_s {
	int size;
	int entries;
	struct key_s **table;
};

static context_t cur_context;
static struct list_s *context;

// TODO have a table of table roots, one for each context and a current context variable
static struct key_map_s *root, *current_map;

static struct key_s *create_key(short, short, context_t, union key_data_u, string_t);
static void destroy_key(struct key_s *);
static struct key_map_s *create_key_map(int);
static void destroy_key_map(struct key_map_s *);

int init_key(void)
{
	if (root)
		return(1);
	if (!(root = create_key_map(KEY_INITIAL_ROOT)))
		return(-1);
	current_map = root;
	return(0);
}

int release_key(void)
{
	destroy_key_map(root);
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
	cur_map = root;
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
				if (!(key = create_key(str[i], 0, context, (union key_data_u) callback, args)))
					return(-1);
			}
			else {
				if (!(map = create_key_map(KEY_INITIAL_SUBMAP)))
					return(-1);
				if (!(key = create_key(str[i], KEY_KBF_SUBMAP, context, (union key_data_u) map, args))) {
					destroy_key_map(map);
					return(-1);
				}
			}
			key->next = cur_map->table[hash];
			cur_map->table[hash] = key;
			cur_map->entries++;
		}
	}
	return(0);
}

/**
 *
 */
int unbind_key(char *context, char *key)
{

}

/**
 *
 */
int process_key(int ch)
{
	struct key_s *cur;

	if (ch >= MAX_KEYS) {
		// TODO process some other way (these are funny keys like resize)
	}

	cur = current_map->table[key_hash(current_map, ch)];
	while (cur) {
		if (cur->ch == ch) {
			if (cur->bitflags & KEY_KBF_SUBMAP)
				current_map = cur->data.submap;
			else {
				execute_callback(cur->data.callback, cur->args);
				current_map = root;
			}
			return(0);
		}
		cur = cur->next;
	}
	// TODO default action (or else return -1 causing the caller to execute default action)
	return(-1);
}

/*** Local Functions ***/

/**
 * Create a new key struct with all the given parameters.
 */
static struct key_s *create_key(short ch, short bitflags, context_t context, union key_data_u data, string_t args)
{
	struct key_s *key;

	if (!(key = (struct key_s *) memory_alloc(sizeof(struct key_s))))
		return(NULL);
	key->ch = ch;
	key->bitflags = bitflags;
	key->context = context;
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
static struct key_map_s *create_key_map(int size)
{
	struct key_map_s *map;

	if (!(map = (struct key_map_s *) memory_alloc(sizeof(struct key_map_s))))
		return(NULL);
	if (!(map->table = (struct key_s **) memory_alloc(size * sizeof(struct key_s *)))) {
		memory_free(map);
		return(NULL);
	}
	map->size = size;
	map->entries = 0;
	return(map);
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

