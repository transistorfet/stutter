/*
 * Module Name:		terminal.c
 * Version:		0.1
 * Interface:		surface
 * Module Requirements:	memory ; linear ; surface
 * System Requirements:	Windows OS
 * Description:		Windows Terminal Manager
 */

#include <windows.h>

#include <stdarg.h>
#include <stdlib.h>

#include CONFIG_H
#include <stutter/lib/memory.h>
#include <stutter/lib/linear.h>
#include <stutter/frontend/surface.h>
#include <stutter/frontend/keycodes.h>
#include "terminal.h"

#define TERMINAL_DEFAULT_WIDTH		80
#define TERMINAL_DEFAULT_HEIGHT		25

#define TERMINAL_DEFAULT_FONT_SIZE	16

struct terminal_s {
	struct surface_s surface;
	short charx;
	short chary;
	attrib_t attrib;
	HWND window;
	HDC context;
	HBITMAP bitmap;
	linear_node_v(terminal_s) list;
};

extern HINSTANCE this_instance;

struct terminal_s *terminal;

static HFONT terminal_font;
static HFONT terminal_bold_font;
static linear_list_v(terminal_s) terminal_list;

struct surface_type_s terminal_type = {
	"windows-terminal",
	0,
	(surface_create_t) terminal_create,
	(surface_destroy_t) terminal_destroy,
	(surface_print_t) terminal_print,
	(surface_clear_t) terminal_clear,
	(surface_move_t) terminal_move,
	(surface_control_t) terminal_control
};

static void terminal_set_attribs(struct terminal_s *, attrib_t);

/**
 * Initialize the terminal and return 0 on success.
 */
int init_terminal(void)
{
	if (terminal)
		return(1);
	linear_init_v(terminal_list);

	// TODO fix font width problem
	terminal_font = (HFONT) GetStockObject(SYSTEM_FIXED_FONT);
	terminal_bold_font = terminal_font;
//	terminal_font = CreateFont(TERMINAL_DEFAULT_FONT_SIZE, 0, 0, 0, FW_NORMAL, 0, 0, 0, 0, 0, 0, 0, FIXED_PITCH, "system");
//	terminal_bold_font = CreateFont(TERMINAL_DEFAULT_FONT_SIZE, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, FIXED_PITCH, "system");

	if (!(terminal = terminal_create(NULL, -1, -1, 0)))
		return(-1);
	return(0);
}

/**
 * Release all terminal resources and return 0 on success.
 */
int release_terminal(void)
{
/*
	linear_destroy_list_v(terminal_list, list,
		// TODO terminal_destroy will attempt to remove it from the list?
		terminal_destroy(cur);
	);
*/
	return(0);
}

/**
 *
 */
struct terminal_s *terminal_create(struct terminal_s *parent, short width, short height, int bitflags)
{
	HDC hdc;
	RECT size;
	TEXTMETRIC tm;
	struct terminal_s *terminal;

	if (!(terminal = memory_alloc(sizeof(struct terminal_s))))
		return(NULL);
	terminal->surface.type = &terminal_type;
	terminal->surface.bitflags = bitflags;
	terminal->surface.x = 0;
	terminal->surface.y = 0;
	terminal->surface.width = (width == -1) ? TERMINAL_DEFAULT_WIDTH : width;
	terminal->surface.height = (height == -1) ? TERMINAL_DEFAULT_HEIGHT : height;

	terminal->window = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		"Stutter",
		"Stutter",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		this_instance,
		NULL
	);

	hdc = GetDC(terminal->window);
	terminal->context = CreateCompatibleDC(hdc);
	terminal->bitmap = CreateCompatibleBitmap(hdc, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
	SelectObject(terminal->context, terminal->bitmap);
	SelectObject(terminal->context, (HBRUSH) GetStockObject(WHITE_BRUSH));
	PatBlt(terminal->context, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), PATCOPY);
	SelectObject(terminal->context, terminal_font);
	GetTextMetrics(terminal->context, &tm);
	terminal->charx = tm.tmMaxCharWidth;
	terminal->chary = tm.tmHeight + tm.tmExternalLeading;
	ReleaseDC(terminal->window, hdc);

	terminal_control(terminal, SCC_MODIFY_ATTRIB, SA_NORMAL, SA_WHITE, SA_BLACK);
	terminal_control(terminal, SCC_RESIZE, terminal->surface.width, terminal->surface.height);

	ShowWindow(terminal->window, SW_RESTORE);
	linear_add_node_v(terminal_list, list, terminal);

	return(terminal);
}

/**
 *
 */
int terminal_destroy(struct terminal_s *terminal)
{
	DestroyWindow(terminal->window);
	return(0);
}

/**
 * Print the given string to the given length (or the whole string if
 * the given length is 0) to the given terminal and return 0 on success.
 */
int terminal_print(struct terminal_s *terminal, char *str, int length)
{
	int i;
	char *tmp;
	char buffer[STRING_SIZE];

	if (length == -1)
		length = strlen(str);
	TextOut(terminal->context, terminal->surface.x * terminal->charx, terminal->surface.y * terminal->chary, str, length);
	terminal->surface.x += (length % terminal->surface.width);
	terminal->surface.y += (length / terminal->surface.width);
	if (terminal->surface.y > terminal->surface.height)
		terminal->surface.y = terminal->surface.y % terminal->surface.height;
	return(0);
}

/**
 * Fill an area of the terminal of the given size and starting at the given
 * x,y coordinates with a blank character.
 */
void terminal_clear(struct terminal_s *terminal, short x, short y, short width, short height)
{
	int i;
	char buffer[512];

	// TODO replace this with FillRect(terminal_context, &rect, HBRUSH?)
	if (!width)
		width = terminal->surface.width - x;
	if (!height)
		height = terminal->surface.height - y;

	for (i = 0;i <= width;i++)
		buffer[i] = ' ';

	for (i = 0;i < height;i++) {
		TextOut(terminal->context, x * terminal->charx, y * terminal->chary, buffer, width);
		y++;
	}
}

/**
 * Move the cursor of the given terminal to the given x, y coordinates.
 */
void terminal_move(struct terminal_s *terminal, short x, short y)
{
	terminal->surface.x = (x < terminal->surface.width) ? x : terminal->surface.width - 1;
	terminal->surface.y = (y < terminal->surface.height) ? y : terminal->surface.height - 1;
}

/**
 *
 */
int terminal_control(struct terminal_s *terminal, int cmd, ...)
{
	va_list va;

	va_start(va, cmd);
	switch (cmd) {
		case SCC_GET_ATTRIB: {
			attrib_t *attrib;

			attrib = va_arg(va, attrib_t *);
			*attrib = terminal->attrib;
			return(0);
		}
		case SCC_SET_ATTRIB: {
			attrib_t *attrib;

			attrib = va_arg(va, attrib_t *);
			terminal->attrib = *attrib;
			terminal_set_attribs(terminal, terminal->attrib);
			return(0);
		}
		case SCC_MODIFY_ATTRIB: {
			int arg;

			arg = va_arg(va, int);
			if (arg != -1)
				terminal->attrib.attrib = arg;
			arg = va_arg(va, int);
			if (arg != -1)
				terminal->attrib.fg = arg;
			arg = va_arg(va, int);
			if (arg != -1)
				terminal->attrib.bg = arg;
			terminal_set_attribs(terminal, terminal->attrib);
			return(0);
		}
		case SCC_RESIZE: {
			int arg;
			RECT size;

			arg = va_arg(va, int);
			if (arg != -1)
				terminal->surface.width = arg;
			arg = va_arg(va, int);
			if (arg != -1)
				terminal->surface.height = arg;
			if (terminal->surface.x >= terminal->surface.width)
				terminal->surface.x = terminal->surface.width - 1;
			if (terminal->surface.y >= terminal->surface.height)
				terminal->surface.y = terminal->surface.height - 1;
			size.top = 0;
			size.left = 0;
			size.right = terminal->surface.width * terminal->charx;
			size.bottom = terminal->surface.height * terminal->chary;
			AdjustWindowRectEx(&size, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_CLIENTEDGE);
			SetWindowPos(terminal->window, NULL, 0, 0, size.right - size.left, size.bottom - size.top, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
			return(0);
		}
		default:
			break;
	}
	va_end(va);
	return(-1);
}

int terminal_free(struct terminal_s *terminal)
{
	linear_remove_node_v(terminal_list, list, terminal);
	ReleaseDC(terminal->window, terminal->context);
	memory_free(terminal);
	return(0);
}

/**
 * Redraw and refresh the terminal by calling all registered refresh callback
 * functions and return 0 on success.
 */
void terminal_refresh(struct terminal_s *terminal)
{
	HDC hdc;
	RECT rect;
	PAINTSTRUCT ps;

	hdc = BeginPaint(terminal->window, &ps);
	GetClientRect(terminal->window, &rect);
	BitBlt(hdc, 0, 0, rect.right, rect.bottom, terminal->context, 0, 0, SRCCOPY);
	EndPaint(terminal->window, &ps);
}

/**
 * Process the resizing message so that the window is always an even number of
 * characters wide and high.
 */
void terminal_resizing(struct terminal_s *terminal, RECT *rect, int dir)
{
	RECT size;

	size.top = 0;
	size.left = 0;
	size.right = 100;
	size.bottom = 100;
	AdjustWindowRectEx(&size, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_CLIENTEDGE);
	size.right -= 100;
	size.bottom -= 100;

	rect->top -= size.top;
	rect->left -= size.left;
	rect->right += size.right;
	rect->bottom += size.bottom;

	terminal->surface.width = (rect->right - rect->left) / terminal->charx;
	terminal->surface.height = (rect->bottom - rect->top) / terminal->chary;

	if (terminal->surface.x >= terminal->surface.width)
		terminal->surface.x = terminal->surface.width - 1;
	if (terminal->surface.y >= terminal->surface.height)
		terminal->surface.y = terminal->surface.height - 1;

	if ((dir == WMSZ_LEFT) || (dir == WMSZ_TOPLEFT) || (dir == WMSZ_BOTTOMLEFT))
		rect->left = rect->right - (terminal->surface.width * terminal->charx);
	else
		rect->right = rect->left + (terminal->surface.width * terminal->charx);

	if ((dir == WMSZ_TOP) || (dir == WMSZ_TOPLEFT) || (dir == WMSZ_TOPRIGHT))
		rect->top = rect->bottom - (terminal->surface.height * terminal->chary);
	else
		rect->bottom = rect->top + (terminal->surface.height * terminal->chary);

	AdjustWindowRectEx(rect, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_CLIENTEDGE);
}

/**
 * Convert windows virtual keys to the common keycodes
 */
int terminal_convert_char(int ch)
{
	switch (ch) {
		case 0x08:
			return(KC_BACKSPACE);
		case VK_RETURN:
			return(KC_ENTER);
		case VK_UP:
			return(KC_UP);
		case VK_DOWN:
			return(KC_DOWN);
		case VK_RIGHT:
			return(KC_RIGHT);
		case VK_LEFT:
			return(KC_LEFT);
		default:
			return(-1);
	}
}

struct terminal_s *terminal_find(HWND window)
{
	struct terminal_s *terminal;

	linear_find_node_v(terminal_list, list, terminal, (cur->window == window));
	return(terminal);
}

HWND terminal_get_window(struct terminal_s *terminal)
{
	return(terminal->window);
}

int terminal_get_number(void)
{
	int i = 0;

	linear_traverse_list_v(terminal_list, list,
		i++;
	);
	return(i);
}


/*** Local Functions ***/

/**
 * Set the current attributes of the given terminal to the given attributes.
 */
static void terminal_set_attribs(struct terminal_s *terminal, attrib_t attrib)
{
	int fg, bg;

	if (attrib.attrib & SA_BOLD)
		SelectObject(terminal->context, terminal_bold_font);
	else
		SelectObject(terminal->context, terminal_font);

	if (attrib.attrib & SA_INVERSE) {
		bg = attrib.fg;
		fg = attrib.bg;
	}
	else {
		fg = attrib.fg;
		bg = attrib.bg;
	}

	SetTextColor(terminal->context, RGB( (fg & 0xff0000) >> 16, (fg & 0xff00) >> 8, (fg & 0xff) ));
	SetBkColor(terminal->context, RGB( (bg & 0xff0000) >> 16, (bg & 0xff00) >> 8, (bg & 0xff) ));
}


