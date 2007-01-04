/*
 * Widget Name:		input.c
 * Version:		0.1
 * Module Requirements:	queue ; memory ; string ; surface
 * Description:		Input Field Widget
 */

#include <string.h>
#include <stdarg.h>

#include CONFIG_H
#include <stutter/lib/queue.h>
#include <stutter/lib/memory.h>
#include <stutter/lib/string.h>
#include <stutter/frontend/widget.h>
#include <stutter/frontend/surface.h>
#include <stutter/frontend/keycodes.h>
#include "input.h"
#include "window.h"

struct widget_type_s input_type = {
	"input:window",
	0,
	sizeof(struct input_s),
	(widget_init_t) input_init,
	(widget_release_t) input_release,
	(widget_refresh_t) input_refresh,
	(widget_print_t) input_print,
	(widget_clear_t) input_clear,
	(widget_read_t) input_read,
	(widget_control_t) input_control
};

static int input_insert_char(struct input_s *, char);
static int input_delete_char(struct input_s *);
static int input_save_buffer(struct input_s *);
static int input_process_char(struct input_s *, int);

int input_init(struct input_s *input)
{
	if (!(input->buffer = (char *) memory_alloc(FE_INPUT_BUFFER_SIZE)))
		return(-1);
	input->i = 0;
	input->end = 0;
	input->max = FE_INPUT_BUFFER_SIZE;

	if (!(input->history = create_queue(FE_INPUT_HISTORY_SIZE, (destroy_t) destroy_string))) {
		memory_free(input->buffer);
		return(-1);
	}
	window_init(WINDOW_S(input));
	return(0);
}

int input_release(struct input_s *input)
{
	destroy_queue(input->history);
	memory_free(input->buffer);
	window_release(WINDOW_S(input));
	return(0);
}

int input_refresh(struct input_s *input)
{
	int i;

	input->buffer[input->end] = '\0';
	surface_clear_m(WINDOW_S(input)->surface, WINDOW_S(input)->x, WINDOW_S(input)->y, WINDOW_S(input)->width, WINDOW_S(input)->height);
	surface_move_m(WINDOW_S(input)->surface, WINDOW_S(input)->x, WINDOW_S(input)->y);
	i = input->i - WINDOW_S(input)->width + 1;
	surface_print_m(WINDOW_S(input)->surface, &input->buffer[(i >= 0) ? i : 0], (input->end >= WINDOW_S(input)->width) ? WINDOW_S(input)->width - 1 : -1);
	surface_control_m(WINDOW_S(input)->surface, SCC_MOVE_CURSOR, ( (input->i >= WINDOW_S(input)->width) ? WINDOW_S(input)->width - 1 : input->i ), WINDOW_S(input)->y);
	return(0);
}

int input_print(struct input_s *input, const char *str, int len)
{
	if (len == -1)
		len = strlen(str);
	if (len > input->max)
		len = input->max;
	strncpy(input->buffer, str, len);
	input->i = input->end = len;
	return(len);
}

void input_clear(struct input_s *input)
{
	input_save_buffer(input);
	input->i = 0;
	input->end = 0;
}

int input_read(struct input_s *input, char *buffer, int max)
{
	strncpy(buffer, input->buffer, max - 1);
	buffer[max - 1] = '\0';
	return(0);
}

int input_control(struct input_s *input, int cmd, va_list va)
{
	switch (cmd) {
		case WCC_GET_MIN_MAX_SIZE: {
			widget_size_t *min, *max;
			min = va_arg(va, widget_size_t *);
			max = va_arg(va, widget_size_t *);
			if (min) {
				min->width = 1;
				min->height = 1;
			}
			if (max) {
				max->width = -1;
				max->height = (WINDOW_S(input)->height > 0) ? WINDOW_S(input)->height : 1;
			}
			return(0);
		}
		case WCC_SCROLL: {
			int i, amount;
			char *str = NULL;

			amount = va_arg(va, int);
			if (amount >= 0) {
				for (i = 0;i < amount;i++) {
					if (!(str = queue_next(input->history)))
						str = queue_first(input->history);
				}
			}
			else {
				amount *= -1;
				for (i = 0;i < amount;i++) {
					if (!(str = queue_previous(input->history)))
						str = queue_last(input->history);
				}
			}
			if (str)
				input_print(input, str, -1);
			break;
		}
		case WCC_INSERT_CHAR: {
			int ch;
			ch = va_arg(va, int);
			input_insert_char(input, ch);
			return(0);
		}
		case WCC_PROCESS_CHAR: {
			int ch;
			ch = va_arg(va, int);
			input_process_char(input, ch);
			return(0);
		}
		default:
			return(window_control(WINDOW_S(input), cmd, va));
	}
	return(-1);
}

/*** Local Functions ***/

/**
 * Insert the given char into the buffer of the given input structure at
 * the location pointed to by the input's index and update the pointers.
 */ 
static int input_insert_char(struct input_s *input, char ch)
{
	int i;

	if (input->end >= input->max)
		return(-1);
	for (i = input->end;i >= input->i;i--)
		input->buffer[i + 1] = input->buffer[i];
	input->buffer[input->i] = ch;
	input->i++;
	input->end++;
	return(0);
}

/**
 * Delete the character at the index of the given input structure.
 */
static int input_delete_char(struct input_s *input)
{
	int i;

	if (input->i <= 0)
		return(-1);
	for (i = input->i;i < input->end;i++)
		input->buffer[i - 1] = input->buffer[i];
	input->i--;
	input->end--;
	return(0);
}

/**
 * Transfer the current input buffer into the history queue list of
 * the given input structure.
 */
static int input_save_buffer(struct input_s *input)
{
	char *str;

	input->buffer[input->end] = '\0';
	if (str = create_string("%s", input->buffer))
		queue_prepend(input->history, str);
	input->history->current = NULL;
	return(0);
}

/**
 * Process the given character and perform the corresponding default
 * behaviour on the given input structure.  If the character is
 * processed then a 0 is returned otherwise a 1 is returned.
 */
static int input_process_char(struct input_s *input, int ch)
{
	switch (ch) {
		case KC_BACKSPACE: {
			input_delete_char(input);
			break;
		}
		case KC_UP: {
			char *str;
			if ((str = queue_next(input->history)) || (str = queue_first(input->history)))
				input_print(input, str, -1);
			break;
		}
		case KC_DOWN: {
			char *str;
			if ((str = queue_previous(input->history)) || (str = queue_last(input->history)))
				input_print(input, str, -1);
			break;
		}
		case KC_RIGHT: {
			if (input->i < input->end)
				input->i++;
			break;
		}
		case KC_LEFT: {
			if (input->i > 0)
				input->i--;
			break;
		}
		default: {
			if ((ch < 0x20) || (ch > 0xff))
				return(1);
			input_insert_char(input, (char) ch);
			break;
		}
	}
	return(0);
}

