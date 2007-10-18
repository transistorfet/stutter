/*
 * Header Name:		key.h
 * Description:		Key Manager Header
 */

#ifndef _STUTTER_KEY_H
#define _STUTTER_KEY_H

#include <stutter/type.h>
#include <stutter/string.h>

int init_key(void);
int release_key(void);

int bind_key(char *, int *, void *, struct type_s *, char *);
int unbind_key(char *, int *);
int process_key(int ch);
int select_key_context(char *);

#endif

