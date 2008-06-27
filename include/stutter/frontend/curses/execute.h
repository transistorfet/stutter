/*
 * Header Name:		execute.h
 * Description:		TCP Network Interface Manager Header
 */

#ifndef _STUTTER_FRONTEND_CURSES_EXECUTE_H
#define _STUTTER_FRONTEND_CURSES_EXECUTE_H

#include <stdarg.h>

#include <stutter/object.h>
#include <stutter/globals.h>
#include <stutter/frontend/curses/interface.h>

extern struct fe_interface_type fe_execute_type;

int fe_execute_init(struct fe_interface *, const char *, va_list);
void fe_execute_release(struct fe_interface *);

int fe_execute_read(struct fe_interface *, char *, int);
int fe_execute_write(struct fe_interface *, const char *, int);

#endif

