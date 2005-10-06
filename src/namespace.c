/*
 * Module Name:		namespace.c
 * Version:		0.1
 * Module Requirements:	list ; string ; memory
 * Description:		Type Manager
 */


#include <string.h>

#include <namespace.h>
#include <nit/list.h>
#include <nit/string.h>
#include <nit/memory.h>
#include <nit/callback.h>

static struct namespace_s *current = NULL;
static struct list_s *namespace_list = NULL;

static int compare_namespace(struct namespace_s *, char *);
static void destroy_namespace(struct namespace_s *);

int init_namespace(void)
{
	if (namespace_list)
		return(1);
	if (!(namespace_list = create_list(0, (compare_t) compare_namespace, (destroy_t) destroy_namespace)))
		return(-1);
	return(0);
}

int release_namespace(void)
{
	if (namespace_list)
		destroy_list(namespace_list);
	return(0);
}

/**
 * Add a namespace to the list with the given name and with the given
 * list (Note: the exact list given is used.  A 0 is returned on
 * success or -1 on error.
 */
struct namespace_s *add_namespace(char *name, struct list_s *list)
{
	struct namespace_s *namespace;

	if (!name || !(namespace = memory_alloc(sizeof(struct namespace_s) + strlen(name) + 1)))
		return(NULL);
	namespace->name = (char *) (((unsigned int) namespace) + sizeof(struct namespace_s));
	strcpy(namespace->name, name);
	namespace->list = list;

	if (list_add(namespace_list, namespace)) {
		destroy_namespace(namespace);
		return(NULL);
	}
	return(namespace);
}

/**
 * The namespace with the given name is removed from the namespace list
 * and its callbacks are destroyed.  If the removal is successful,
 * a 0 is returned, otherwise -1.
 */
int remove_namespace(char *name)
{
	return(list_delete(namespace_list, name));
}

/**
 * Select the namespace with the given name as the current namespace to
 * be used as the default namespace for lookups that do not include a
 * namespace reference.  If the given namespace cannot be found, -1 is
 * returned and the current namespace is not changed otherwise 0 is returned.
 */
int select_namespace(char *name)
{
	struct namespace_s *namespace;

	if (!(namespace = list_find(namespace_list, name, 0)))
		return(-1);
	current = namespace;
	return(0);
}

/**
 * Return a pointer to the currently selected or return NULL if no namespace
 * is currently selected.
 */
struct namespace_s *current_namespace(void)
{
	return(current);
}

/**
 * Look up the namespace with the given name and return a pointer to the
 * namespace or return NULL if the namespace has not been defined.
 */
struct namespace_s *find_namespace(char *name)
{
	return(list_find(namespace_list, name, 0));
}

/**
 * Look up the namespace using the give name interpreted as a
 * variable name with the namespace encoded in it (the
 * namespace:variable format).  If no namespace is present then
 * the default namespace is returned.
 */
struct namespace_s *resolve_namespace(char *name)
{
	if (!strchr(name, ':'))
		// TODO return the current namesapce
		return(NULL);
	return(list_find(namespace_list, name, 0));
}

/*** Local Functions ***/

/**
 * Compare the name of the given namespace with the given name
 * and return 0 if they match or nonzero otherwise.
 */
static int compare_namespace(struct namespace_s *namespace, char *name)
{
	return(strcmp(namespace->name, name));
/*
	int i = 0;

	while (1) {
		if ((namespace->name[i] == '\0') && ((name[i] == ':') || name[i] == '\0'))
			return(0);
		else if ((name[i] == '\0') || (namespace->name[i] != name[i]))
			return(1);
		i++;
	}
	return(1);
*/
}

/**
 * Free all resources used by the given namespace.
 */
static void destroy_namespace(struct namespace_s *namespace)
{
	destroy_list(namespace->list);
	memory_free(namespace);
}


