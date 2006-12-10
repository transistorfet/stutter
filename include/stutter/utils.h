/*
 * Header Name:		utils.h
 * Version:		0.1
 * Description:		Utilities Header
 */

#ifndef _STUTTER_UTILS_H
#define _STUTTER_UTILS_H

int util_escape_char(char *, char *);
int util_convert_key(char *, char *, int);
int util_expand_str(char *, char *, int);
int util_expand_variable(char *, char *, int, int *);
int util_emit_str(char *, void *, char *, ...);
int util_evaluate_command(char *, char *);

#endif


