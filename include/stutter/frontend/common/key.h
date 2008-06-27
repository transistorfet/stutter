/*
 * Header Name:		key.h
 * Description:		Key Manager Header
 */

#ifndef _STUTTER_FRONTEND_COMMON_KEY_H
#define _STUTTER_FRONTEND_COMMON_KEY_H

#include <stutter/object.h>
#include <stutter/variable.h>

int init_key(void);
int release_key(void);

int fe_key_bind(const char *, const int *, struct variable_s *, const char *);
int fe_key_unbind(const char *, const int *);
int fe_key_process(int);
int fe_key_select_context(const char *);

#endif

