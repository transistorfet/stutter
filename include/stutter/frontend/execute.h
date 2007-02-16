/*
 * Header Name:		execute.h
 * Version:		0.1
 * Description:		Frontend Program Execution Header
 */

#ifndef _STUTTER_FRONTEND_EXECUTE_H
#define _STUTTER_FRONTEND_EXECUTE_H

#include <stutter/globals.h>

#define FE_EXEC_BF_EMIT_SIGNAL

typedef void *fe_execute_t;

fe_execute_t fe_execute_open(char *cmd, int bitflags);
int fe_execute_close(fe_execute_t exec);

struct callback_s fe_execute_get_callback(fe_execute_t exec);
void fe_execute_set_callback(fe_execute_t exec, int condition, callback_t func, void *ptr);

int fe_execute_send(fe_execute_t exec, char *buffer, int len);
int fe_execute_receive(fe_execute_t exec, char *buffer, int len);
int fe_execute_receive_str(fe_execute_t exec, char *buffer, int len, char ch);


#endif

