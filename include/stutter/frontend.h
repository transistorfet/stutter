/*
 * Header Name:		frontend.h
 * Version:		0.1
 * Description:		Frontend Header
 */

#ifndef _STUTTER_FRONTEND_H
#define _STUTTER_FRONTEND_H

#include <stutter/lib/string.h>
#include <stutter/lib/globals.h>

void *fe_create_widget(char *ns, char *type, char *id, void *parent);
int fe_destroy_widget(void *widget);
void *fe_get_parent(void *widget);
short fe_get_width(void *widget);
short fe_get_height(void *widget);
void *fe_find_widget(char *id);
int fe_resize_widget(void *widget, int diffx, int diffy);

void *fe_current_widget(char *context, void *ref);
int fe_select_widget(char *context, void *ref, void *widget);
void *fe_next_widget(char *context, void *ref);
void *fe_previous_widget(char *context, void *ref);
void *fe_first_widget(char *context, void *ref);
void *fe_last_widget(char *context, void *ref);

int fe_print(void *widget, const char *str);
char *fe_read(void *widget, char *buffer, int max);
void fe_clear(void *widget);
void fe_move(void *widget, short x, short y);
int fe_scroll(void *widget, int diff);
void fe_refresh(void *widget);

void fe_quit(char *reason);

#endif


