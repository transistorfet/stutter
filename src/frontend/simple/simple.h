/*
 * Header Name:		simple.h
 * Version:		0.1
 * Description:		Front End Definition Header
 */

#ifndef _FRONTEND_SIMPLE_H
#define _FRONTEND_SIMPLE_H

#include <stdio.h>
#include <stdlib.h>
#include <nit/list.h>
#include <nit/queue.h>
#include <nit/string.h>
#include <nit/screen.h>
#include "input.h"

extern int exit_flag;
extern struct input_s *input;
extern struct list_s *key_list;
extern struct screen_s *screen;
extern struct queue_s *window_list;

//#define fe_create_widget(type, parent)		create_widget(100)	//(char *, void *) -> void *
void *fe_create_widget(char *, void *);
//#define fe_destroy_widget(widget)		destroy_widget(widget)	//(void *) -> int
int fe_destroy_widget(void *);
#define fe_get_parent(widget)			NULL	//(void *) -> void *
//#define fe_get_width(widget)			((struct widget_s *) widget)->width	//(void *) -> short
short fe_get_width(void *);
//#define fe_get_height(widget)			((struct widget_s *) widget)->height	//(void *) -> short
short fe_get_height(void *);

#define fe_current_widget()			queue_current(window_list)	//() -> void *
#define fe_set_current_widget(widget)		queue_find(window_list, widget)	//(void *) -> int
#define fe_next_widget()			queue_next(window_list)	//() -> void *
#define fe_previous_widget()			queue_previous(window_list)	//() -> void *
#define fe_first_widget()			queue_first(window_list)	//() -> void *
#define fe_last_widget()			queue_last(window_list)	//() -> void *

//#define fe_print(widget, str)			((struct widget_s *) widget)->type->print(widget, str)	//(void *, string_t) -> int
int fe_print(void *, string_t);
//#define fe_clear(widget)			//(void *) -> void
void fe_clear(void *);
//#define fe_move(widget, cx, cy)			//(void *, short, short) -> void
void fe_move(void *, short, short);
//#define fe_scroll(widget, amount)		//(void *, int) -> int
int fe_scroll(void *, int);
#define fe_refresh()				refresh_screen(screen)	//() -> void

#define fe_get_input_buffer()			input_buffer_m(input)	//() -> char *
#define fe_set_input_buffer(str)		input_set_buffer(input, str)	//(char *) -> int
#define fe_clear_input()			clear_input(input)	//() -> void

//#define fe_register_widget(name, callback)	//(char *, callback_t) -> int
int fe_register_widget(char *, struct callback_s *);
#define fe_unregister_widget(name)		//(char *) -> int

#define fe_bind_key(context, key, func, ptr)	list_add(key_list, create_callback(0, 0, (void *) key, (callback_t) func, (void *) ptr));

#define fe_terminate()				exit_flag = 0	//() -> void

#endif


