/*
 * Header Name:		base.h
 * Version:		0.1
 * Description:		Base Module Header
 */

#ifndef _STUTTER_MODULES_BASE_H
#define _STUTTER_MODULES_BASE_H

#include <stdlib.h>

#include <stutter/type.h>
#include <stutter/string.h>

int init_base(void);
int release_base(void);

/* Commands */
int base_cmd_alias(char *, char *);
int base_cmd_bind(char *, char *);
int base_cmd_clear(char *, char *);
int base_cmd_clearline(char *, char *);
int base_cmd_close(char *, char *);
int base_cmd_echo(char *, char *);
int base_cmd_evaluate(char *, char *);
int base_cmd_exec(char *, char *);
int base_cmd_expand(char *, char *);
int base_cmd_get(char *, char *);
int base_cmd_parse(char *, char *);
int base_cmd_quit(char *, char *);
int base_cmd_redirect(char *, char *);
int base_cmd_remove(char *, char *);
int base_cmd_scroll(char *, char *);
int base_cmd_select(char *, char *);
int base_cmd_set(char *, char *);
int base_cmd_source(char *, char *);
int base_cmd_timer(char *, char *);
int base_cmd_unbind(char *, char *);

/* Signal Handlers */
int base_sig_print(void *, void *, char *);

/* Types */
struct type_s *base_load_command(void);
struct type_s *base_load_format(void);
struct type_s *base_load_status(void);
struct type_s *base_load_string(void);
struct type_s *base_load_string_ref(void);
struct type_s *base_load_table(void);
struct type_s *base_load_time(void);
struct type_s *base_load_timer(void);
struct type_s *base_load_wildcard(void);

#endif


