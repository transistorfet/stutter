/*
 * Header Name:		namespace.h
 * Version:		0.1
 * Description:		Namespace Manager Header
 */

#ifndef _NAMESPACE_H
#define _NAMESPACE_H

#include <nit/list.h>

struct namespace_s {
	char *name;
	struct list_s *list;
};

int init_namespace(void);
int release_namespace(void);

struct namespace_s *add_namespace(char *, struct list_s *);
int remove_namespace(char *);
struct namespace_s *find_namespace(char *);

#endif


