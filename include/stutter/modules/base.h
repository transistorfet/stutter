/*
 * Header Name:		base.h
 * Version:		0.1
 * Description:		Base Module Header
 */

#ifndef _BASE_H
#define _BASE_H

#include <type.h>
#include <nit/string.h>

int init_base(void);
int release_base(void);

/* Commands */
int base_cmd_bind(char *, char *);
int base_cmd_next(char *, char *);
int base_cmd_previous(char *, char *);
int base_cmd_clear(char *, char *);
int base_cmd_scroll(char *, char *);
int base_cmd_get(char *, char *);
int base_cmd_set(char *, char *);
int base_cmd_remove(char *, char *);
int base_cmd_parse(char *, char *);

/* Types */
struct type_s *base_load_format(void);
struct type_s *base_load_command(void);

/* Utils */
string_t base_expand_str(char *);

#endif


