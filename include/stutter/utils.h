/*
 * Header Name:		utils.h
 * Description:		Utilities Header
 */

#ifndef _STUTTER_UTILS_H
#define _STUTTER_UTILS_H

int util_atoi(const char *, int);
int util_itoa(int, char *, int);
int util_encode_bytes(unsigned char *, int, char *, int);
int util_decode_bytes(const char *, unsigned char *, int);
int util_escape_char(const char *, char *);
int util_convert_key(const unsigned char *, int *, int);
int util_expand_str(const char *, char *, int);
int util_expand_variable(const char *, char *, int, int *);
int util_evaluate_command(const char *, char *);
char *util_get_arg(char *, int *);

#endif


