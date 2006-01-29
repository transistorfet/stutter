/*
 * Module Name:		key.c
 * Version:		0.1
 * Module Requirements:	variable ; hash ; memory ; string
 * Description:		Key Manager
 */

#include <string.h>

#include <key.h>
#include <variable.h>
#include <lib/hash.h>
#include <lib/memory.h>
#include <lib/string.h>

#ifndef KEY_INITIAL_CONTEXTS
#define KEY_INITIAL_CONTEXTS	10
#endif

#ifndef KEY_INITIAL_ROOT
#define KEY_INITIAL_ROOT	20
#endif

#ifndef KEY_INITIAL_SUBMAP
#define KEY_INITIAL_SUBMAP	10
#endif

#ifndef KEY_LOAD_FACTOR
#define KEY_LOAD_FACTOR		0.75
#endif

#define KEY_KBF_SUBMAP		0x01

#define context_hash_m(list, key)	( sdbm_hash(key) % hash_size_v(list) )
#define context_compare_m(key)		( !strcmp(cur->context, key) )

#define key_hash_m(list, key)		( ((unsigned int) (key)) % hash_size_v(list) )
#define key_compare_m(key)		( cur->ch == key )

struct key_map_s {
	string_t context;
	hash_list_v(key_s) kl;
	hash_node_v(key_map_s) cl;
};

struct key_s {
	short ch;
	short bitflags;
	union key_data_u {
		struct variable_s *variable;
		struct key_map_s *submap;
	} data;
	string_t args;
	hash_node_v(key_s) kl;
};

static hash_list_v(key_map_s) context_list;
static struct key_map_s *current_root, *current_map;

static struct key_map_s *key_add_context(char *);
static struct key_map_s *key_find_context(char *);
static struct key_s *create_key(short, short, union key_data_u, string_t);
static void destroy_key(struct key_s *);
static void keymap_add_key(struct key_map_s *, char, struct key_s *);
static struct key_map_s *create_key_map(char *, int);
static void destroy_key_map(struct key_map_s *);

int init_key(void)
{
	struct key_map_s *root;

	if (current_root)
		return(1);
	hash_init_v(context_list, KEY_INITIAL_CONTEXTS);
	if (!(root = key_add_context(""))) {
		release_key();
		return(-1);
	}
	current_root = root;
	current_map = root;
	return(0);
}

int release_key(void)
{
	hash_destroy_list_v(context_list, cl,
		destroy_key_map(cur);
	);
	hash_release_v(context_list);
	return(0);
}

/**
 * Associate the given variable and arguments  with the given
 * key sequence under the given context.  If the key cannot be bound,
 * -1 will be returned.  If the key sequence is already bound, the
 * old binding will be destroyed and replace with the given variable
 * and arguments.  A 0 is returned on success.
 */
int bind_key(char *context, char *str, struct variable_s *variable, string_t args)
{
	int i;
	struct key_s *key, *cur_key;
	struct key_map_s *map, *cur_map;

	if ((*str == '\0') || !variable)
		return(-1);
	if (!(cur_map = context ? key_find_context(context) : current_root) && !(cur_map = key_add_context(context)))
		return(-1);
	for (i = 0;str[i] != '\0';i++) {
		hash_find_node_v(cur_map->kl, kl, cur_key, key_hash_m(cur_map->kl, str[i]), key_compare_m(str[i]));
		if (str[i + 1] == '\0') {
			if (!cur_key) {
				/* Create a new terminating key entry */
				if (!(key = create_key(str[i], 0, (union key_data_u) variable, args)))
					return(-1);
				keymap_add_key(cur_map, str[i], key);
			}
			else if (cur_key->bitflags & KEY_KBF_SUBMAP) {
				/* Fail (Don't overwrite a submap with a single key) */
				return(-1);
			}
			else {
				/* Overwrite this key with our new key information (Rebind) */
				if (cur_key->args)
					destroy_string(cur_key->args);
				cur_key->bitflags = 0;
				cur_key->data.variable = variable;
				cur_key->ch = str[i];
				cur_key->args = args;
			}
		}
		else {
			if (!cur_key) {
				/* Create a new submap */
				if (!(map = create_key_map(NULL, KEY_INITIAL_SUBMAP)))
					return(-1);
				if (!(key = create_key(str[i], KEY_KBF_SUBMAP, (union key_data_u) map, NULL))) {
					destroy_key_map(map);
					return(-1);
				}
				keymap_add_key(cur_map, str[i], key);
			}
			else if (!(cur_key->bitflags & KEY_KBF_SUBMAP)) {
				/* Overwrite this key with a new submap */
				if (!(map = create_key_map(NULL, KEY_INITIAL_SUBMAP)))
					return(-1);
				if (cur_key->args)
					destroy_string(cur_key->args);
				cur_key->data.submap = map;
				cur_key->bitflags = KEY_KBF_SUBMAP;
				cur_key->ch = str[i];
				cur_key->args = args;
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
int unbind_key(char *context, char *str)
{
	int i;
	struct key_s *cur_key;
	struct key_map_s *cur_map;

	if (!(cur_map = context ? key_find_context(context) : current_root))
		return(-1);
	for (i = 0;str[i] != '\0';i++) {
		hash_find_node_v(cur_map->kl, kl, cur_key, key_hash_m(cur_map->kl, str[i]), key_compare_m(str[i]));
		if (!cur_key)
			return(1);
		else if (str[i + 1] == '\0') {
			/* Remove the entry (whether a submap or terminating key entry) */
			hash_remove_node_v(cur_map->kl, kl, cur_key, key_hash_m(cur_map->kl, str[i]), key_compare_m(str[i]));
			destroy_key(cur_key);
		}
		else if (cur_key->bitflags & KEY_KBF_SUBMAP) {
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
int process_key(int ch)
{
	string_t args;
	struct key_s *node;

	hash_find_node_v(current_map->kl, kl, node, key_hash_m(current_map->kl, ch), key_compare_m(ch));
	if (!node)
		return(-1);
	if (node->bitflags & KEY_KBF_SUBMAP)
		current_map = node->data.submap;
	else {
		if (!(args = duplicate_string(node->args)))
			return(-1);
		node->data.variable->type->evaluate(node->data.variable->value, args);
		destroy_string(args);
		current_map = current_root;
	}
	return(0);
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

	if (!(root = key_find_context(context)) && !(root = key_add_context(context)))
		return(-1);
	current_root = root;
	return(0);
}

/*** Local Functions ***/

static struct key_map_s *key_add_context(char *context)
{
	struct key_map_s *node;

	if (!(node = create_key_map(context, KEY_INITIAL_ROOT)))
		return(NULL);
	hash_add_node_v(context_list, cl, node, context_hash_m(context_list, context));
	if (hash_load_v(context_list) > KEY_LOAD_FACTOR)
		hash_rehash_v(context_list, cl, (hash_size_v(context_list) * 1.75), key_hash_m(context_list, cur->context));
	return(node);
}

static struct key_map_s *key_find_context(char *context)
{
	struct key_map_s *node;

	hash_find_node_v(context_list, cl, node, context_hash_m(context_list, context), context_compare_m(context));
	return(node);
}

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
		if (key->args)
			destroy_string(key->args);
		memory_free(key);
	}
}


static void keymap_add_key(struct key_map_s *map, char key, struct key_s *node)
{
	hash_add_node_v(map->kl, kl, node, key_hash_m(map->kl, key));
	if (hash_load_v(map->kl) > KEY_LOAD_FACTOR)
		hash_rehash_v(map->kl, kl, (hash_size_v(map->kl) * 1.75), key_hash_m(map->kl, cur->ch));
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
	map->context = context ? create_string(context) : NULL;
	hash_init_v(map->kl, size);
	return(map);
}

/**
 * Free the resources used by the given map and all of its submaps
 * and key bindings.  A 0 is returned on success or -1 on failure.
 */
static void destroy_key_map(struct key_map_s *map)
{
	hash_destroy_list_v(map->kl, kl,
		destroy_key(cur);
	);
	hash_release_v(map->kl);
}



