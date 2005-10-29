/*
 * Header Name:		key.h
 * Version:		0.1
 * Description:		Key Manager Header
 */

#ifndef _KEY_H
#define _KEY_H

#include <nit/string.h>
#include <nit/callback.h>

int init_key(void);
int release_key(void);

int bind_key(char *context, char *key, struct callback_s *, string_t);
int unbind_key(char *context, char *key);
int process_key(int ch);
int select_key_context(char *);

#endif

