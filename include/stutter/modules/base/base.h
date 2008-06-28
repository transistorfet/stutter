/*
 * Header Name:		base.h
 * Description:		Base Module Header
 */

#ifndef _STUTTER_MODULES_BASE_H
#define _STUTTER_MODULES_BASE_H

#include <stutter/modules/base/types.h>

int init_base(void);
int release_base(void);

/* Commands */
int base_cmd_alias(char *, char *);
int base_cmd_concat(char *, char *);
int base_cmd_evaluate(char *, char *);
int base_cmd_exec(char *, char *);
int base_cmd_expand(char *, char *);
int base_cmd_get(char *, char *);
int base_cmd_parse(char *, char *);
int base_cmd_quit(char *, char *);
int base_cmd_redirect(char *, char *);
int base_cmd_remove(char *, char *);
int base_cmd_set(char *, char *);
int base_cmd_source(char *, char *);
int base_cmd_timer(char *, char *);

#endif


