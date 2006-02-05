/*
 * Module Name:		input.c
 * Version:		0.1
 * Module Requirements:	queue ; memory ; string ; screen
 * Description:		Input Buffer
 */

#include <string.h>
#include <curses.h>

#include <lib/queue.h>
#include <lib/memory.h>
#include <lib/string.h>
#include "screen.h"
#include "input.h"

/**
 * Allocate and initialize a input structure given the intialization values.
 */
struct input_s *create_input(int size, int history)
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

	input->max_history = history;
	queue_init_v(input->history);

	return(input);
}

/**
 * Free all the resources used by the given input structure.
 */
int destroy_input(struct input_s *input)
{
	queue_destroy_v(input->history, history,
		if (cur->cmd)
			destroy_string(cur->cmd);
		memory_free(cur);
	);
	memory_free(input);
	return(0);
 }

/**
 * Redraw the given input buffer on the screen.
 */
int refresh_input(struct input_s *input)
{
	int i;
	short width, height;

	width = screen_width();
	height = screen_height();

	input->buffer[input->end] = '\0';
	screen_clear(0, height - 1, width, 1);
	screen_move(0, height - 1);
	i = input->i - width + 1;
	screen_print(&input->buffer[(i >= 0) ? i : 0], 0);
	screen_move(( input->i >= width ? width - 1 : input->i ), height - 1);
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
//	char *str;

	input->buffer[input->end] = '\0';
//	if (str = create_string(input->buffer))
//		queue_append(input->history, str);
	return(0);
}

/**
 * Return the current input buffer.
 */
char *input_get_buffer(struct input_s *input)
{
	char *str;
	struct input_history_s *node;

	if ((str = create_string(input->buffer)) && (node = (struct input_history_s *) memory_alloc(sizeof(struct input_history_s)))) {
		node->cmd = str;
		queue_append_node_v(input->history, node, history);
		if (queue_size_v(input->history) > input->max_history) {
			queue_pop_node_v(input->history, node, history);
			if (node) {
				if (node->cmd)
					destroy_string(node->cmd);
				memory_free(node);
			}
		}
	}
	return(input->buffer);
}

/**
 * Erase the current contents of the buffer of the given input structure
 * and replace it with the given string.
 */
int input_set_buffer(struct input_s *input, char *str)
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
		case KEY_BACKSPACE:
			input_delete_char(input);
			break;
		case KEY_UP:
			// TODO fix for new queue
			if (str = queue_previous(input->history))
				input_set_buffer(input, str);
			break;
		case KEY_DOWN:
			// TODO fix for new queue
			if (str = queue_next(input->history))
				input_set_buffer(input, str);
			break;
		case KEY_RIGHT:
			if (input->i < input->end)
				input->i++;
			break;
		case KEY_LEFT:
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

