/*
 * Widget Name:		input.c
 * Description:		Input Field Widget
 */

#include <string.h>
#include <stdarg.h>

#include CONFIG_H
#include <stutter/queue.h>
#include <stutter/memory.h>
#include <stutter/macros.h>
#include <stutter/string.h>
#include <stutter/frontend/keycodes.h>
#include <stutter/frontend/common/key.h>
#include <stutter/frontend/common/widget.h>
#include <stutter/frontend/common/surface.h>
#include <stutter/frontend/common/widgets/input.h>
#include <stutter/frontend/common/widgets/window.h>

struct fe_widget_type fe_input_type = { {
	OBJECT_TYPE_S(&fe_window_type),
	"fe_input",
	sizeof(struct fe_input),
	NULL,
	(object_init_t) fe_input_init,
	(object_release_t) fe_input_release },
	(fe_widget_write_t) fe_input_write,
	(fe_widget_read_t) fe_input_read,
	(fe_widget_refresh_t) fe_input_refresh,
	(fe_widget_clear_t) fe_input_clear,
	(fe_widget_control_t) fe_input_control
};

static inline int fe_input_insert_char(struct fe_input *, char);
static inline int fe_input_delete_char(struct fe_input *);
static inline int fe_input_save_buffer(struct fe_input *);
static inline int fe_input_process_char(struct fe_input *, int);

int fe_input_init(struct fe_input *input, const char *params, va_list va)
{
	char *context = NULL;

	if (fe_window_init(FE_WINDOW(input), params, va))
		return(-1);
	if (!(input->buffer = (char *) memory_alloc(FE_INPUT_BUFFER_SIZE)))
		return(-1);
	// TODO add context stuff
	//context = get_property(props, "context");
	input->context = create_string("%s", context ? context : "global");
	input->target = FE_WIDGET(input)->parent;
	input->i = 0;
	input->end = 0;
	input->max = FE_INPUT_BUFFER_SIZE;

	if (!(input->history = create_queue(FE_INPUT_HISTORY_SIZE, (destroy_t) destroy_string)))
		return(-1);
	FE_WIDGET(input)->bitflags |= WBF_NEEDS_REFRESH;
	return(0);
}

void fe_input_release(struct fe_input *input)
{
	if (input->buffer)
		memory_free(input->buffer);
	if (input->history)
		destroy_queue(input->history);
}

int fe_input_write(struct fe_input *input, const char *str, int len)
{
	if (len == -1)
		len = strlen(str);
	if (len > input->max)
		len = input->max;
	strncpy(input->buffer, str, len);
	input->i = input->end = len;
	return(len);
}

int fe_input_read(struct fe_input *input, char *buffer, int max)
{
	strncpy(buffer, input->buffer, max - 1);
	buffer[max - 1] = '\0';
	return(strlen(buffer));
}

int fe_input_refresh(struct fe_input *input, struct fe_surface *surface)
{
	int i;

	input->buffer[input->end] = '\0';
	FE_SURFACE_CLEAR(surface, FE_WINDOW(input)->x, FE_WINDOW(input)->y, FE_WINDOW(input)->width, FE_WINDOW(input)->height);
	FE_SURFACE_MOVE(surface, FE_WINDOW(input)->x, FE_WINDOW(input)->y);
	i = input->i - FE_WINDOW(input)->width + 1;
	FE_SURFACE_PRINT(surface, &input->buffer[(i >= 0) ? i : 0], (input->end >= FE_WINDOW(input)->width) ? FE_WINDOW(input)->width - 1 : -1);
	FE_SURFACE_CONTROL(surface, SCC_MOVE_CURSOR, ( (input->i >= FE_WINDOW(input)->width) ? FE_WINDOW(input)->width - 1 : input->i ), FE_WINDOW(input)->y);
	return(0);
}

void fe_input_clear(struct fe_input *input, struct fe_surface *surface)
{
	fe_input_save_buffer(input);
	input->i = 0;
	input->end = 0;
}

int fe_input_control(struct fe_input *input, int cmd, va_list va)
{
	switch (cmd) {
		case WCC_SET_FOCUS: {
			struct fe_widget *widget;

			widget = va_arg(va, struct fe_widget *);
			if (widget && (FE_WIDGET(input) != widget))
				return(-1);
			else if (FE_WIDGET(input)->parent) {
				if (fe_widget_control(FE_WIDGET(input)->parent, WCC_SET_FOCUS, input))
					return(-1);
				// TODO reinstate this when you fix up keybinding
				//select_key_context(input->context);
			}
			return(0);
		}
		case WCC_GET_FOCUS: {
			struct fe_widget **widget;

			widget = va_arg(va, struct fe_widget **);
			if (widget)
				*widget = FE_WIDGET(input);
			return(0);
	 	}
		case WCC_GET_TARGET: {
			struct fe_widget **widget;

			// TODO should this do anything like ask for the target of the target?
			widget = va_arg(va, struct fe_widget **);
			if (widget)
				*widget = input->target;
			return(0);
		}
		case WCC_SET_TARGET: {
			input->target = va_arg(va, struct fe_widget *);
			return(0);
		}
		case WCC_SET_PARENT: {
			struct fe_widget *parent;

			parent = va_arg(va, struct fe_widget *);
			FE_WIDGET(input)->parent = parent;
			input->target = parent;
			fe_widget_control(FE_WIDGET(input), WCC_SET_FOCUS, NULL);
			return(0);
		}
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
				max->height = (FE_WINDOW(input)->height > 0) ? FE_WINDOW(input)->height : 1;
			}
			return(0);
		}
		case WCC_SCROLL: {
			int i, amount;

			amount = va_arg(va, int);
			if (amount >= 0) {
				for (i = 0; i < amount; i++) {
					if (!(QUEUE_NEXT(input->history)))
						QUEUE_FIRST(input->history);
				}
			}
			else {
				amount *= -1;
				for (i = 0; i < amount; i++) {
					if (!(QUEUE_PREV(input->history)))
						QUEUE_LAST(input->history);
				}
			}
			if (QUEUE_CURRENT(input->history))
				fe_input_write(input, (char *) QUEUE_CURRENT(input->history), -1);
			break;
		}
		case WCC_INSERT_CHAR: {
			int ch;
			ch = va_arg(va, int);
			fe_input_insert_char(input, ch);
			return(0);
		}
		case WCC_PROCESS_CHAR: {
			int ch;
			ch = va_arg(va, int);
			fe_input_process_char(input, ch);
			return(0);
		}
		default:
			return(fe_window_control(FE_WINDOW(input), cmd, va));
	}
	return(-1);
}

/*** Local Functions ***/

/**
 * Insert the given char into the buffer of the given input structure at
 * the location pointed to by the input's index and update the pointers.
 */ 
static inline int fe_input_insert_char(struct fe_input *input, char ch)
{
	int i;

	if (input->end >= input->max)
		return(-1);
	for (i = input->end; i >= input->i; i--)
		input->buffer[i + 1] = input->buffer[i];
	input->buffer[input->i] = ch;
	input->i++;
	input->end++;
	return(0);
}

/**
 * Delete the character at the index of the given input structure.
 */
static inline int fe_input_delete_char(struct fe_input *input)
{
	int i;

	if (input->i <= 0)
		return(-1);
	for (i = input->i; i < input->end; i++)
		input->buffer[i - 1] = input->buffer[i];
	input->i--;
	input->end--;
	return(0);
}

/**
 * Transfer the current input buffer into the history queue list of
 * the given input structure.
 */
static inline int fe_input_save_buffer(struct fe_input *input)
{
	string_t str;

	input->buffer[input->end] = '\0';
	if (!(str = create_string("%s", input->buffer)))
		return(-1);
	queue_prepend(input->history, str);
	QUEUE_SET_CURRENT_ENTRY(input->history, NULL);
	return(0);
}

/**
 * Process the given character and perform the corresponding default
 * behaviour on the given input structure.  If the character is
 * processed then a 0 is returned otherwise a 1 is returned.
 */
static inline int fe_input_process_char(struct fe_input *input, int ch)
{
	switch (ch) {
		case KC_BACKSPACE: {
			fe_input_delete_char(input);
			break;
		}
		case KC_UP: {
			if (QUEUE_NEXT(input->history) || QUEUE_FIRST(input->history))
				fe_input_write(input, (char *) QUEUE_CURRENT(input->history), -1);
			break;
		}
		case KC_DOWN: {
			if (QUEUE_PREV(input->history) || QUEUE_LAST(input->history))
				fe_input_write(input, (char *) QUEUE_CURRENT(input->history), -1);
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
			fe_input_insert_char(input, (char) ch);
			break;
		}
	}
	return(0);
}

