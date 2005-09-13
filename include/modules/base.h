/*
 * Header Name:		base.h
 * Version:		0.1
 * Description:		Base Module Header
 */

#ifndef _BASE_H
#define _BASE_H

/* Commands */
int base_cmd_next(char *, char *);
int base_cmd_previous(char *, char *);
int base_cmd_scroll(char *, char *);
int base_cmd_get(char *, char *);
int base_cmd_set(char *, char *);

/* Keys */

/* Utils */
int base_parse_command(char *env, int ch);

#endif


