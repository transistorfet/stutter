/*
 * Header Name:		namespace.h
 * Version:		0.1
 * Description:		Namespace Manager Header
 */

#ifndef _NAMESPACE_H
#define _NAMESPACE_H

int init_namespace(void);
int release_namespace(void);

int add_namespace(char *, int);
int remove_namespace(char *);

#endif


