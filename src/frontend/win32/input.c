/*
 * Module Name:		input.c
 * Version:		0.1
 * Module Requirements:	queue ; memory ; string ; surface
 * Description:		Input Buffer
 */

#include <string.h>
#include <windows.h>

#include <stutter/lib/queue.h>
#include <stutter/lib/memory.h>
#include <stutter/lib/string.h>
#include <stutter/frontend/surface.h>
#include <stutter/frontend/keycodes.h>
#include "input.h"

/**
 * Allocate and initialize a input structure given the intialization values.
 */
struct input_s *create_input(struct surface_s *surface, int size, int history)
{
	struct input_s *input;

	if (!size)
		size = INPUT_MAX_BUFFER;
	if (!(input = (struct input_s *) memory_alloc(sizeof(struct input_s) + size)))
		return(NULL);
	memset(input, '\0', sizeof(struct input_s));
	input->i = 0;
	input->end = 0;
	input->max = size;
	input->buffer = (char *) (((size_t) input) + sizeof(struct input_s));
	input->surface = surface;

	if (!(input->history = create_queue(history, (destroy_t) destroy_string))) {
		memory_free(input);
		return(NULL);
	}

	return(input);
}

/**
 * Free all the resources used by the given input structure.
 */
int destroy_input(struct input_s *input)
{
	destroy_queue(input->history);
	memory_free(input);
	return(0);
 }

/**
 * Redraw the given input buffer on the terminal.
 */
int refresh_input(struct input_s *input)
{
	int i;
	short width, height;

	width = surface_get_width_m(input->surface);
	height = surface_get_height_m(input->surface);

	input->buffer[input->end] = '\0';
	surface_clear_m(input->surface, 0, height - 1, width, 1);
	surface_move_m(input->surface, 0, height - 1);
	i = input->i - width + 1;
	surface_print_m(input->surface, &input->buffer[(i >= 0) ? i : 0], (input->end >= width) ? width - 1 : -1);
	surface_move_m(input->surface, ( input->i >= width ? width - 1 : input->i ), height - 1);
	return(0);
}

/**
 * Insert the given char into the buffer of the given input structure at
 * the location pointed to by the input's index and update the pointers.
 */ 
int input_insert_char(struct input_s *input, char ch)
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
int input_delete_char(struct input_s *input)
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
 * Clear the buffer of the given input structure.
 */
int clear_input(struct input_s *input)
{
	input_save_buffer(input);
	input->i = 0;
	input->end = 0;
	return(0);
}

/**
 * Transfer the current input buffer into the history queue list of
 * the given input structure.
 */
int input_save_buffer(struct input_s *input)
{
	char *str;

	input->buffer[input->end] = '\0';
	if (str = create_string(input->buffer))
		queue_append(input->history, str);
	input->history->current = NULL;
	return(0);
}

/**
 * Return the current input buffer.
 */
char *input_get_buffer(struct input_s *input, char *buffer, int max)
{
	strncpy(buffer, input->buffer, max - 1);
	buffer[max - 1] = '\0';
	return(buffer);
}

/**
 * Erase the current contents of the buffer of the given input structure
 * and replace it with the given string.
 */
int input_set_buffer(struct input_s *input, const char *str)
{
	strncpy(input->buffer, str, input->max);
	input->i = input->end = strlen(str);
	return(0);
}

/**
 * Process the given character and perform the corresponding default
 * behaviour on the given input structure.  If the character is
 * processed then a 0 is returned otherwise a 1 is returned.
 */
int input_default(struct input_s *input, int ch)
{
	char *str;

	switch (ch) {
		case KC_BACKSPACE:
			input_delete_char(input);
			break;
		case KC_UP:
			if ((str = queue_previous(input->history)) || (str = queue_last(input->history)))
				input_set_buffer(input, str);
			break;
		case KC_DOWN:
			if ((str = queue_next(input->history)) || (str = queue_first(input->history)))
				input_set_buffer(input, str);
			break;
		case KC_LEFT:
			if (input->i < input->end)
				input->i++;
			break;
		case KC_RIGHT:
			if (input->i > 0)
				input->i--;
			break;
		default:
			if (ch >= 0x20 && ch <= 0xff)
				input_insert_char(input, (char) ch);
			else
				return(1);
			break;	
	}
	return(0);
}

