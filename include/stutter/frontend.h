/*
 * Header Name:		frontend.h
 * Version:		0.1
 * Description:		Frontend Header
 */

#ifndef _FRONTEND_H
#define _FRONTEND_H

#include <nit/string.h>
#include <nit/callback.h>

void *fe_create_widget(char *, void *);
int fe_destroy_widget(void *);
void *fe_get_parent(void *);
short fe_get_width(void *);
short fe_get_height(void *);

// TODO what argument will we pass to all of these functions in order to specify a "context" (a string? a widget?)
void *fe_current_widget(void);
int fe_set_current_widget(void *);
void *fe_next_widget(void);
void *fe_previous_widget(void);
void *fe_first_widget(void);
void *fe_last_widget(void);

int fe_print(void *, string_t);
void fe_clear(void *);
void fe_move(void *, short, short);
int fe_scroll(void *, int);
void fe_refresh(void);

// TODO should this function return the acutal buffer or a string_t?
char *fe_get_input(void);
int fe_set_input(char *);
void fe_clear_input(void);

// TODO there should only be one way to specify a callback and that method should be consistent (register vs bind_key)
// TODO should there be more callbacks or something passed to fe_register_widget
int fe_register_widget(char *, struct callback_s *);
int fe_unregister_widget(char *);

// TODO is this good or should it *really* be fe_exit()?
void fe_terminate(void);

#endif


