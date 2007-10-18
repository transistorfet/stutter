/*
 * Header Name:		frontend.h
 * Description:		Frontend Header
 */

#ifndef _STUTTER_FRONTEND_H
#define _STUTTER_FRONTEND_H

#include <stutter/string.h>
#include <stutter/globals.h>

void *fe_create_widget(char *ns, char *type, char *id, void *parent);
int fe_destroy_widget(void *widget);
void *fe_get_parent(void *widget);
short fe_get_width(void *widget);
short fe_get_height(void *widget);
void *fe_find_widget(char *id);
int fe_resize_widget(void *widget, int x, int y);

void *fe_get_focus(char *type);
int fe_set_focus(void *widget);
void *fe_get_target(void *widget, char *type);
int fe_set_target(void *widget);
int fe_show_widget(void *widget);

int fe_print(void *widget, const char *str);
int fe_read(void *widget, char *buffer, int max);
void fe_clear(void *widget);
void fe_move(void *widget, short x, short y);
int fe_scroll(void *widget, int diff);

void fe_quit(char *reason);

#endif


