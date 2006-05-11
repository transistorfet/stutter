/*
 * Header Name:		input.h
 * Version:		0.1
 * Description:		Input Buffer Header
 */

#ifndef _SIMPLE_INPUT_H
#define _SIMPLE_INPUT_H

#include <stutter/lib/queue.h>
#include <stutter/lib/string.h>

#ifndef INPUT_MAX_BUFFER
#define INPUT_MAX_BUFFER		1024
#endif

struct input_s {
	int i;
	int end;
	int max;
	char *buffer;
	struct queue_s *history;
};

#define input_buffer_m(input)		input->buffer

struct input_s *create_input(int, int);
int destroy_input(struct input_s *);
int refresh_input(struct input_s *);

int input_insert_char(struct input_s *, char);
int input_delete_char(struct input_s *);
int clear_input(struct input_s *);
int input_save_buffer(struct input_s *);
char *input_get_buffer(struct input_s *, char *, int);
int input_set_buffer(struct input_s *, char *);

int input_default(struct input_s *, int);

#endif


