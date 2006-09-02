/*
 * Widget Name:		input.c
 * Version:		0.1
 * Module Requirements:	queue ; memory ; string ; surface
 * Description:		Input Field Widget
 */

#include <string.h>
#include <stdarg.h>

#include <stutter/lib/queue.h>
#include <stutter/lib/memory.h>
#include <stutter/lib/string.h>
#include <stutter/frontend/widget.h>
#include <stutter/frontend/surface.h>
#include <stutter/frontend/keycodes.h>
#include "input.h"
#include "window.h"

#define INPUT_MAX_BUFFER		1024
#define INPUT_MAX_HISTORY		100

struct widget_type_s input_type = {
	"input",
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
	if (!(input->buffer = (char *) memory_alloc(INPUT_MAX_BUFFER)))
		return(-1);
	input->i = 0;
	input->end = 0;
	input->max = INPUT_MAX_BUFFER;

	if (!(input->history = create_queue(INPUT_MAX_HISTORY, (destroy_t) destroy_string))) {
		memory_free(input->buffer);
		return(-1);
	}

	window_init((struct window_s *) input);
	return(0);
}

int input_release(struct input_s *input)
{
	destroy_queue(input->history);
	memory_free(input->buffer);
	return(0);
 }

int input_refresh(struct input_s *input)
{
	int i;

	input->buffer[input->end] = '\0';
	surface_clear_m(input->window.surface, input->window.x, input->window.y, input->window.width, input->window.height);
	surface_move_m(input->window.surface, input->window.x, input->window.y);
	i = input->i - input->window.width + 1;
	surface_print_m(input->window.surface, &input->buffer[(i >= 0) ? i : 0], (input->end >= input->window.width) ? input->window.width - 1 : -1);
	// TODO how will you set the cursor
	surface_move_m(input->window.surface, ( input->i >= input->window.width ? input->window.width - 1 : input->i ), input->window.height - 1);
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
		case WCC_PROCESS_CHAR: {
			int ch;
			ch = va_arg(va, int);
			input_process_char(input, ch);
			return(0);
		}
		default:
			return(window_control((struct window_s *) input, cmd, va));
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
	if (str = create_string(input->buffer))
		queue_append(input->history, str);
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
	char *str;

	switch (ch) {
		case KC_BACKSPACE:
			input_delete_char(input);
			break;
		case KC_UP:
			if ((str = queue_previous(input->history)) || (str = queue_last(input->history)))
				input_print(input, str, -1);
			break;
		case KC_DOWN:
			if ((str = queue_next(input->history)) || (str = queue_first(input->history)))
				input_print(input, str, -1);
			break;
		case KC_RIGHT:
			if (input->i < input->end)
				input->i++;
			break;
		case KC_LEFT:
			if (input->i > 0)
				input->i--;
			break;
		default:
			if ((ch < 0x20) || (ch > 0xff))
				return(1);
			input_insert_char(input, (char) ch);
			break;	
	}
	return(0);
}

