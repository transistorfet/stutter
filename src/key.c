/*
 * Module Name:		key.c
 * Version:		0.1
 * Module Requirements:	type ; hash ; memory ; string
 * Description:		Key Manager
 */

#include <string.h>

#include CONFIG_H
#include <stutter/key.h>
#include <stutter/type.h>
#include <stutter/hash.h>
#include <stutter/macros.h>
#include <stutter/memory.h>
#include <stutter/string.h>

#define KEY_KBF_SUBMAP		0x01

#define key_map_hash(list, key)	\
	((unsigned int) key)

#define key_map_compare_node(node, ptr)	\
	( node->ch == (int) ptr )

#define context_release_node(list, node)	\
	key_map_release_table(node)

#define context_hash(list, key)	\
	sdbm_hash_icase(key)

#define context_compare_node(node, ptr)	\
	(!strcmp_icase(node->context, key))

struct key_map_s {
	string_t context;
	hash_list_v(key_s) kl;
	hash_node_v(key_map_s) cl;
};

struct key_s {
	int ch;
	short bitflags;
	union key_data_u {
		struct key_variable_s {
			void *value;
			struct type_s *type;
		} variable;
		struct key_map_s *submap;
	} data;
	string_t args;
	hash_node_v(key_s) kl;
};

struct context_list_s {
	hash_list_v(key_map_s) cl;
};

static struct context_list_s context_list;
static struct key_map_s *root_context = NULL, *current_context = NULL, *current_map = NULL;

static inline void key_map_release_node(struct key_map_s *, struct key_s *);
static inline struct key_map_s *create_context(char *);
static inline struct key_s *create_key(short, short, union key_data_u, string_t);
static inline struct key_map_s *create_key_map(char *, int);

DEFINE_HASH_TABLE(key_map, struct key_map_s, struct key_s, kl, ch, key_map_release_node, key_map_hash, key_map_compare_node, KEY_MAP_LOAD_FACTOR, KEY_MAP_GROWTH_FACTOR)
DEFINE_HASH_TABLE(context, struct context_list_s, struct key_map_s, cl, context, context_release_node, context_hash, context_compare_node, KEY_MAP_LOAD_FACTOR, KEY_MAP_GROWTH_FACTOR)

int init_key(void)
{
	if (root_context)
		return(0);
	context_init_table(&context_list, KEY_CONTEXT_LIST_INIT_SIZE);
	if (!(root_context = create_context("global"))) {
		release_key();
		return(-1);
	}
	current_context = root_context;
	current_map = current_context;
	return(0);
}

int release_key(void)
{
	if (root_context)
		return(0);
	context_release_table(&context_list);
	root_context = NULL;
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
int bind_key(char *context, int *key, void *value, struct type_s *type, char *args)
{
	int i;
	union key_data_u data;
	struct key_s *cur_key;
	struct key_map_s *map, *cur_map;

	if ((*key == '\0') || !type || !type->evaluate || !(args = create_string("%s", args)))
		return(-1);
	data.variable.value = value;
	data.variable.type = type;

	if (!(cur_map = context ? context_find_node(&context_list, context) : current_context) && !(cur_map = create_context(context)))
		return(-1);
	for (i = 0;key[i] != '\0';i++) {
		cur_key = key_map_find_node(cur_map, (void *) key[i]);
		if (key[i + 1] == '\0') {
			if (!cur_key) {
				/* Create a new terminating key entry */
				if (!(cur_key = create_key(key[i], 0, data, args)))
					return(-1);
				key_map_add_node(cur_map, cur_key);
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
				cur_key->data = data;
				cur_key->ch = key[i];
				cur_key->args = args;
			}
		}
		else {
			if (!cur_key) {
				/* Create a new submap */
				if (!(map = create_key_map(NULL, KEY_SUBMAP_INIT_SIZE)))
					return(-1);
				if (!(cur_key = create_key(key[i], KEY_KBF_SUBMAP, (union key_data_u) map, NULL))) {
					key_map_destroy_table(map);
					return(-1);
				}
				key_map_add_node(cur_map, cur_key);
			}
			else if (!(cur_key->bitflags & KEY_KBF_SUBMAP)) {
				/* Overwrite this key with a new submap */
				if (!(map = create_key_map(NULL, KEY_SUBMAP_INIT_SIZE)))
					return(-1);
				if (cur_key->args)
					destroy_string(cur_key->args);
				cur_key->data.submap = map;
				cur_key->bitflags = KEY_KBF_SUBMAP;
				cur_key->ch = key[i];
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
int unbind_key(char *context, int *key)
{
	int i;
	struct key_s *cur_key;
	struct key_map_s *cur_map;

	if (!(cur_map = context ? context_find_node(&context_list, context) : current_context))
		return(-1);
	for (i = 0;key[i] != '\0';i++) {
		if (key[i + 1] == '\0') {
			/* Remove the entry (whether a submap or terminating key entry) */
			key_map_delete_node(cur_map, (void *) key[i]);
		}
		else if (!(cur_key = key_map_find_node(cur_map, (void *) key[i]))) {
			/* We didn't find the key sequence */
			return(1);
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

	if (!(node = key_map_find_node(current_map, (void *) ch))) {
		if ((current_map != current_context) || (current_context == root_context)) {
			current_map = current_context;
			return(-1);
		}
		else {
			if (!(node = key_map_find_node(root_context, (void *) ch)))
				return(-1);
			current_map = root_context;
		}
	}

	if (node->bitflags & KEY_KBF_SUBMAP)
		current_map = node->data.submap;
	else {
		if (!(args = duplicate_string(node->args)))
			return(-1);
		node->data.variable.type->evaluate(node->data.variable.value, args);
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
int select_key_context(char *context)
{
	struct key_map_s *root;

	if (!context)
		context = "global";
	if (!(root = context_find_node(&context_list, context)) && !(root = create_context(context)))
		return(-1);
	current_context = root;
	return(0);
}

/*** Local Functions ***/

/**
 * Free the resources used by the given key struct.
 */
static inline void key_map_release_node(struct key_map_s *map, struct key_s *key)
{
	if (key) {
		if (key->bitflags & KEY_KBF_SUBMAP)
			key_map_destroy_table(key->data.submap);
		if (key->args)
			destroy_string(key->args);
	}
}

/**
 * Create a new context of the given name and add it to the context list.
 */
static inline struct key_map_s *create_context(char *context)
{
	struct key_map_s *node;

	if (!(node = create_key_map(context, KEY_ROOTMAP_INIT_SIZE)))
		return(NULL);
	context_add_node(&context_list, node);
	return(node);
}

/**
 * Create a new key struct with all the given parameters.
 */
static inline struct key_s *create_key(short ch, short bitflags, union key_data_u data, string_t args)
{
	struct key_s *key;

	if (!(key = key_map_create_node(sizeof(struct key_s))))
		return(NULL);
	key->ch = ch;
	key->bitflags = bitflags;
	key->data = data;
	key->args = args;

	return(key);
}

/**
 * Create a new key map of the given initial size and return a
 * pointer to it or return NULL on error.
 */
static inline struct key_map_s *create_key_map(char *context, int size)
{
	struct key_map_s *map;

	if (!context)
		context = "global";
	if (!(map = context_create_node(sizeof(struct key_map_s) + strlen(context) + 1)))
		return(NULL);
	map->context = (char *) (((size_t) map) + sizeof(struct key_map_s));
	strcpy(map->context, context);
	key_map_init_table(map, size);
	return(map);
}


