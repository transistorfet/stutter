/*
 * Module Name:		terminal.c
 * System Requirements:	Windows OS
 * Description:		Windows Terminal Manager
 */

#include <windows.h>

#include <stdarg.h>
#include <stdlib.h>

#include CONFIG_H
#include <stutter/key.h>
#include <stutter/type.h>
#include <stutter/memory.h>
#include <stutter/linear.h>
#include <stutter/variable.h>
#include <stutter/frontend.h>
#include <stutter/frontend/widget.h>
#include <stutter/frontend/surface.h>
#include <stutter/frontend/keycodes.h>
#include <stutter/frontend/common/layout.h>
#include <stutter/frontend/common/colourmap.h>
#include "menu.h"
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
	struct menu_s *menu;
	linear_node_v(terminal_s) list;
};

extern HINSTANCE this_instance;

static HFONT terminal_font;
static HFONT terminal_bold_font;
static int terminal_initialized = 0;
static linear_list_v(terminal_s) terminal_list;
static attrib_t terminal_def_attrib = { 0, SA_NORMAL, { SC_ENC_RGBA, SC_RGBA_WHITE }, { SC_ENC_RGBA, SC_RGBA_BLACK } };

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

LRESULT CALLBACK terminal_callback(HWND, UINT, WPARAM, LPARAM);
static struct surface_s *terminal_generate(struct surface_type_s *, struct property_s *, struct layout_s *);
static void terminal_set_attribs(struct terminal_s *, attrib_t);
static inline int terminal_free(struct terminal_s *);
static inline void terminal_paint(struct terminal_s *);
static inline int terminal_resizing(struct terminal_s *, RECT *, int);
static inline int terminal_adjust_size(struct terminal_s *, RECT *);
static inline int terminal_convert_char(int);
static inline int terminal_convert_colour(colour_t);

int init_terminal(void)
{
	WNDCLASSEX winclass;
	struct type_s *type;

	if (terminal_initialized)
		return(0);
	winclass.cbSize = sizeof(WNDCLASSEX);
	winclass.hInstance = this_instance;
	winclass.lpszClassName = "terminal";
	winclass.lpfnWndProc = terminal_callback;
	winclass.cbClsExtra = 0;
	winclass.cbWndExtra = 0;

	winclass.style = 0;
	winclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	winclass.hIconSm = NULL;
	winclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	winclass.lpszMenuName = NULL;
	winclass.hbrBackground = NULL;

	if (!RegisterClassEx(&winclass))
		return(-1);
	linear_init_v(terminal_list);

	// TODO fix font width problem
	terminal_font = (HFONT) GetStockObject(SYSTEM_FIXED_FONT);
	terminal_bold_font = terminal_font;
//	terminal_font = CreateFont(TERMINAL_DEFAULT_FONT_SIZE, 0, 0, 0, FW_NORMAL, 0, 0, 0, 0, 0, 0, 0, FIXED_PITCH, "system");
//	terminal_bold_font = CreateFont(TERMINAL_DEFAULT_FONT_SIZE, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, FIXED_PITCH, "system");

	if (init_colourmap())
		return(-1);
	layout_register_type("terminal", LAYOUT_RT_SURFACE, (layout_create_t) terminal_generate, &terminal_type);
	if (init_menu())
		return(-1);
	if ((type = find_type("colour:fe"))) {
		add_variable(NULL, type, "fe.fg", 0, "pointer", &terminal_def_attrib.fg);
		add_variable(NULL, type, "fe.bg", 0, "pointer", &terminal_def_attrib.bg);
	}
	terminal_initialized = 1;
	return(0);
}

int release_terminal(void)
{
	struct terminal_s *cur, *tmp;

	if (!terminal_initialized)
		return(0);
	layout_unregister_type("terminal");
	linear_foreach_safe_v(terminal_list, list, cur, tmp) {
		destroy_surface(SURFACE_S(cur));
		terminal_free(cur);
	}
	release_menu();
	release_colourmap();
	terminal_initialized = 0;
	return(0);
}

/**
 * Create a new terminal of the given dimensions.
 */
struct terminal_s *terminal_create(struct terminal_s *parent, short width, short height, int bitflags)
{
	HDC hdc;
	TEXTMETRIC tm;
	struct terminal_s *terminal;

	if (!(terminal = memory_alloc(sizeof(struct terminal_s))))
		return(NULL);
	SURFACE_S(terminal)->type = &terminal_type;
	SURFACE_S(terminal)->bitflags = bitflags;
	SURFACE_S(terminal)->x = 0;
	SURFACE_S(terminal)->y = 0;
	SURFACE_S(terminal)->width = (width == -1) ? TERMINAL_DEFAULT_WIDTH : width;
	SURFACE_S(terminal)->height = (height == -1) ? TERMINAL_DEFAULT_HEIGHT : height;
	SURFACE_S(terminal)->root = NULL;

	terminal->window = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		"terminal",
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
	terminal->menu = NULL;

	terminal_control(terminal, SCC_MODIFY_ATTRIB, SA_METHOD_SET, SA_NORMAL, SC_ENC_MAPPING, SC_MAP_DEFAULT_COLOUR, SC_ENC_MAPPING, SC_MAP_DEFAULT_COLOUR);
	terminal_control(terminal, SCC_RESIZE, SURFACE_S(terminal)->width, SURFACE_S(terminal)->height);

	ShowWindow(terminal->window, SW_RESTORE);
	linear_add_node_v(terminal_list, list, terminal);

	return(terminal);
}

/**
 * Destroy the given terminal
 */
int terminal_destroy(struct terminal_s *terminal)
{
	/** Cause a destroy message to be sent to the terminal's window */
	DestroyWindow(terminal->window);
	return(0);
}

/**
 * Print the given string to the given length (or the whole string if
 * the given length is 0) to the given terminal and return 0 on success.
 */
int terminal_print(struct terminal_s *terminal, char *str, int length)
{
	if (length == -1)
		length = strlen(str);
	terminal_set_attribs(terminal, terminal->attrib);
	TextOut(terminal->context, SURFACE_S(terminal)->x * terminal->charx, SURFACE_S(terminal)->y * terminal->chary, str, length);
	SURFACE_S(terminal)->x += (length % SURFACE_S(terminal)->width);
	SURFACE_S(terminal)->y += (length / SURFACE_S(terminal)->width);
	if (SURFACE_S(terminal)->y > SURFACE_S(terminal)->height)
		SURFACE_S(terminal)->y = SURFACE_S(terminal)->y % SURFACE_S(terminal)->height;
	return(0);
}

/**
 * Fill an area of the terminal of the given size and starting at the given
 * x,y coordinates with a blank character.
 */
void terminal_clear(struct terminal_s *terminal, short x, short y, short width, short height)
{
	int i;
	char buffer[STRING_SIZE];

	// TODO replace this with FillRect(terminal_context, &rect, HBRUSH?)
	terminal_set_attribs(terminal, terminal->attrib);
	if (!width)
		width = SURFACE_S(terminal)->width - x;
	if (!height)
		height = SURFACE_S(terminal)->height - y;

	for (i = 0;(i <= width) && (i < STRING_SIZE);i++)
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
	SURFACE_S(terminal)->x = (x < SURFACE_S(terminal)->width) ? x : SURFACE_S(terminal)->width - 1;
	SURFACE_S(terminal)->y = (y < SURFACE_S(terminal)->height) ? y : SURFACE_S(terminal)->height - 1;
}

/**
 * Perform the given command on the given terminal using the given
 * parameters.
 */
int terminal_control(struct terminal_s *terminal, int cmd, ...)
{
	va_list va;

	va_start(va, cmd);
	switch (cmd) {
		case SCC_SET_ROOT: {
			struct widget_s *root, **widget_ptr;

			root = va_arg(va, struct widget_s *);
			widget_ptr = va_arg(va, struct widget_s **);
			if (widget_ptr)
				*widget_ptr = SURFACE_S(terminal)->root;
			else if (SURFACE_S(terminal)->root)
				destroy_widget(SURFACE_S(terminal)->root);
			if ((SURFACE_S(terminal)->root = root)) {
				widget_control(SURFACE_S(terminal)->root, WCC_SET_SURFACE, terminal);
				widget_control(SURFACE_S(terminal)->root, WCC_SET_WINDOW, 0, 0, SURFACE_S(terminal)->width, SURFACE_S(terminal)->height);
			}
			return(0);
		}
		case SCC_REFRESH: {
			InvalidateRect(terminal->window, NULL, 1);
			return(0);
		}
		case SCC_GET_ATTRIB: {
			attrib_t *attrib;

			attrib = va_arg(va, attrib_t *);
			*attrib = terminal->attrib;
			return(0);
		}
		case SCC_SET_ATTRIB: {
			attrib_t *attrib;

			attrib = va_arg(va, attrib_t *);
			if (!attrib)
				return(-1);
			terminal->attrib.style = surface_modify_style(attrib->method, terminal->attrib.style, attrib->style);
			if ((attrib->fg.enc != SC_ENC_MAPPING) || (attrib->fg.colour != SC_MAP_CURRENT_COLOUR))
				terminal->attrib.fg = attrib->fg;
			if ((attrib->bg.enc != SC_ENC_MAPPING) || (attrib->bg.colour != SC_MAP_CURRENT_COLOUR))
				terminal->attrib.bg = attrib->bg;
			return(0);
		}
		case SCC_MODIFY_ATTRIB: {
			int method, enc;
			int arg;

			method = va_arg(va, int);
			arg = va_arg(va, int);
			terminal->attrib.method = 0;
			terminal->attrib.style = surface_modify_style(method, terminal->attrib.style, arg);
			enc = va_arg(va, int);
			arg = va_arg(va, int);
			if ((enc != SC_ENC_MAPPING) || (arg != SC_MAP_CURRENT_COLOUR)) {
				terminal->attrib.fg.enc = enc;
				terminal->attrib.fg.colour = arg;
			}
			enc = va_arg(va, int);
			arg = va_arg(va, int);
			if ((enc != SC_ENC_MAPPING) || (arg != SC_MAP_CURRENT_COLOUR)) {
				terminal->attrib.bg.enc = enc;
				terminal->attrib.bg.colour = arg;
			}
			return(0);
		}
		case SCC_RESIZE: {
			int arg;
			RECT size;

			arg = va_arg(va, int);
			if (arg != -1)
				SURFACE_S(terminal)->width = arg;
			arg = va_arg(va, int);
			if (arg != -1)
				SURFACE_S(terminal)->height = arg;
			if (SURFACE_S(terminal)->x >= SURFACE_S(terminal)->width)
				SURFACE_S(terminal)->x = SURFACE_S(terminal)->width - 1;
			if (SURFACE_S(terminal)->y >= SURFACE_S(terminal)->height)
				SURFACE_S(terminal)->y = SURFACE_S(terminal)->height - 1;
			size.top = 0;
			size.left = 0;
			size.right = SURFACE_S(terminal)->width * terminal->charx;
			size.bottom = SURFACE_S(terminal)->height * terminal->chary;
			terminal_adjust_size(terminal, &size);
			SetWindowPos(terminal->window, NULL, 0, 0, size.right - size.left, size.bottom - size.top, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
			if (SURFACE_S(terminal)->root)
				widget_control(SURFACE_S(terminal)->root, WCC_SET_WINDOW, 0, 0, SURFACE_S(terminal)->width, SURFACE_S(terminal)->height);
			return(0);
		}
		case SCC_MOVE_CURSOR: {
			int x, y;
			x = va_arg(va, int);
			y = va_arg(va, int);
			x = (x < SURFACE_S(terminal)->width) ? ( (x >= 0) ? x : 0 ) : SURFACE_S(terminal)->width - 1;
			y = (y < SURFACE_S(terminal)->height) ? ( (y >= 0) ? y : 0 ) : SURFACE_S(terminal)->height - 1;
			terminal->attrib.style ^= SA_INVERSE;
			terminal_set_attribs(terminal, terminal->attrib);
			TextOut(terminal->context, x * terminal->charx, y * terminal->chary, " ", 1);
			terminal->attrib.style ^= SA_INVERSE;
			return(0);
		}
		default:
			break;
	}
	va_end(va);
	return(-1);
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

/*** Local Functions ***/

LRESULT CALLBACK terminal_callback(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message) {
		case WM_CLOSE: {
			int i = 0;
			struct terminal_s *terminal;

			linear_find_node_v(terminal_list, list, terminal, (cur->window == hwnd));
			linear_foreach_v(terminal_list, list, terminal)
				i++;
			if (i <= 1)
				fe_quit("Lost Terminal");
			else if (terminal)
				DestroyWindow(terminal->window);
			break;
		}
		case WM_DESTROY: {
			struct terminal_s *terminal;

			linear_find_node_v(terminal_list, list, terminal, (cur->window == hwnd));
			terminal_free(terminal);
			break;
		}
		case WM_SETFOCUS: {
			struct terminal_s *terminal;

			linear_find_node_v(terminal_list, list, terminal, (cur->window == hwnd));
			surface_set_current(SURFACE_S(terminal));
			break;
		}
		case WM_KEYDOWN: {
			if ((wparam = terminal_convert_char(wparam)) == -1)
				break;
		}
		case WM_CHAR: {
			struct widget_s *focus;
			struct terminal_s *terminal;

			linear_find_node_v(terminal_list, list, terminal, (cur->window == hwnd));
			if (process_key(wparam) && terminal && (focus = (struct widget_s *) fe_get_focus(NULL)))
				widget_control(focus, WCC_PROCESS_CHAR, wparam);
			InvalidateRect(hwnd, NULL, 1);
			break;
		}
		case WM_SIZING: {
			struct terminal_s *terminal;

			linear_find_node_v(terminal_list, list, terminal, (cur->window == hwnd));
			if (terminal) {
				terminal_resizing(terminal, (RECT *) lparam, wparam);
				InvalidateRect(hwnd, NULL, 1);
			}
			break;
		}
		case WM_PAINT: {
			struct terminal_s *terminal;

			linear_find_node_v(terminal_list, list, terminal, (cur->window == hwnd));
			if (terminal)
				terminal_paint(terminal);
			break;
		}
		case WM_COMMAND: {
			if (!lparam) {
				menu_process_request(LOWORD(wparam));
				InvalidateRect(hwnd, NULL, 1);
			}
			break;
		}
		default:
			return(DefWindowProc(hwnd, message, wparam, lparam));
	}

	return(0);
}

/**
 * Create a surface through the layout generation interface.
 */
static struct surface_s *terminal_generate(struct surface_type_s *type, struct property_s *props, struct layout_s *children)
{
	struct layout_s *cur;
	struct widget_s *widget;
	struct terminal_s *terminal;

	// TODO check the properties for a width and height
	if (!(terminal = (struct terminal_s *) create_surface(type, NULL, -1, -1, 0)))
		return(NULL);

	cur = children;
	while (cur) {
		if ((LAYOUT_RETURN_TYPE(cur->type) == LAYOUT_RT_WIDGET) && !(SURFACE_S(terminal)->root)) {
			widget = layout_call_create_m(cur->type, cur->props, cur->children);
			surface_control_m(terminal, SCC_SET_ROOT, widget, NULL);
		}
		else if ((LAYOUT_RETURN_TYPE(cur->type) == LAYOUT_RT_MENU) && !(terminal->menu)) {
			if ((terminal->menu = layout_call_create_m(cur->type, cur->props, cur->children))) {
				attach_menu(terminal->menu, terminal->window);
				terminal_control(terminal, SCC_RESIZE, SURFACE_S(terminal)->width, SURFACE_S(terminal)->height);
			}
		}
		cur = cur->next;
	}
	return((struct surface_s *) terminal);
}

/**
 * Set the current attributes of the given terminal to the given attributes.
 */
static void terminal_set_attribs(struct terminal_s *terminal, attrib_t attrib)
{
	colour_t fg, bg;

	if ((attrib.fg.enc == SC_ENC_MAPPING) && (attrib.fg.colour == SC_MAP_DEFAULT_COLOUR))
		attrib.fg = terminal_def_attrib.fg;
	if ((attrib.bg.enc == SC_ENC_MAPPING) && (attrib.bg.colour == SC_MAP_DEFAULT_COLOUR))
		attrib.bg = terminal_def_attrib.bg;

	if (attrib.style & SA_BOLD)
		SelectObject(terminal->context, terminal_bold_font);
	else
		SelectObject(terminal->context, terminal_font);

	if (attrib.style & SA_INVERSE) {
		bg = attrib.fg;
		fg = attrib.bg;
	}
	else {
		fg = attrib.fg;
		bg = attrib.bg;
	}

	SetTextColor(terminal->context, terminal_convert_colour(fg));
	SetBkColor(terminal->context, terminal_convert_colour(bg));
}

/**
 * Free the resources of the given terminal
 */
static inline int terminal_free(struct terminal_s *terminal)
{
	linear_remove_node_v(terminal_list, list, terminal);
	if (SURFACE_S(terminal)->root)
		destroy_widget(WIDGET_S(SURFACE_S(terminal)->root));
	if (terminal->menu)
		destroy_menu(terminal->menu);
	ReleaseDC(terminal->window, terminal->context);
	remove_signal(terminal, NULL);
	memory_free(terminal);
	return(0);
}

/**
 * Redraw and refresh the terminal by calling all registered refresh callback
 * functions and return 0 on success.
 */
static inline void terminal_paint(struct terminal_s *terminal)
{
	HDC hdc;
	RECT rect;
	PAINTSTRUCT ps;

	if (SURFACE_S(terminal)->root)
		widget_refresh_m(SURFACE_S(terminal)->root);
	hdc = BeginPaint(terminal->window, &ps);
	GetClientRect(terminal->window, &rect);
	BitBlt(hdc, 0, 0, rect.right, rect.bottom, terminal->context, 0, 0, SRCCOPY);
	EndPaint(terminal->window, &ps);
}

/**
 * Process the resizing message so that the window is always an even number of
 * characters wide and high.
 */
static inline int terminal_resizing(struct terminal_s *terminal, RECT *rect, int dir)
{
	RECT size;
	int ret = 0;
	int width, height;

	size.top = 0;
	size.left = 0;
	size.right = 100;
	size.bottom = 100;
	terminal_adjust_size(terminal, &size);
	size.right -= 100;
	size.bottom -= 100;

	rect->top -= size.top;
	rect->left -= size.left;
	rect->right += size.right;
	rect->bottom += size.bottom;

	width = (rect->right - rect->left) / terminal->charx;
	height = (rect->bottom - rect->top) / terminal->chary;
	if ((SURFACE_S(terminal)->width != width) || (SURFACE_S(terminal)->height != height))
		ret = 1;
	SURFACE_S(terminal)->width = width;
	SURFACE_S(terminal)->height = height;

	if (SURFACE_S(terminal)->x >= SURFACE_S(terminal)->width)
		SURFACE_S(terminal)->x = SURFACE_S(terminal)->width - 1;
	if (SURFACE_S(terminal)->y >= SURFACE_S(terminal)->height)
		SURFACE_S(terminal)->y = SURFACE_S(terminal)->height - 1;

	if ((dir == WMSZ_LEFT) || (dir == WMSZ_TOPLEFT) || (dir == WMSZ_BOTTOMLEFT))
		rect->left = rect->right - (SURFACE_S(terminal)->width * terminal->charx);
	else
		rect->right = rect->left + (SURFACE_S(terminal)->width * terminal->charx);

	if ((dir == WMSZ_TOP) || (dir == WMSZ_TOPLEFT) || (dir == WMSZ_TOPRIGHT))
		rect->top = rect->bottom - (SURFACE_S(terminal)->height * terminal->chary);
	else
		rect->bottom = rect->top + (SURFACE_S(terminal)->height * terminal->chary);

	terminal_adjust_size(terminal, rect);
	if (SURFACE_S(terminal)->root)
		widget_control(SURFACE_S(terminal)->root, WCC_SET_WINDOW, 0, 0, SURFACE_S(terminal)->width, SURFACE_S(terminal)->height);
	return(ret);
}

/**
 * Adjust the given rectangle such that when the function returns, rectangle
 * holds the window size necessary to contain a client area of the size
 * the rectangle was when the function was called.
 */
static inline int terminal_adjust_size(struct terminal_s *terminal, RECT *rect)
{
	WINDOWINFO info;

	info.cbSize = sizeof(WINDOWINFO);
	GetWindowInfo(terminal->window, &info);
	AdjustWindowRectEx(rect, info.dwStyle, GetMenu(terminal->window) ? TRUE : FALSE, info.dwExStyle);
	return(0);
}

/**
 * Convert windows virtual keys to the common keycodes
 */
static inline int terminal_convert_char(int ch)
{
	switch (ch) {
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

/**
 * Convert the given colour and encoding into an RGB colour
 */
static inline int terminal_convert_colour(colour_t colour)
{
	if (colour.enc == SC_ENC_MAPPING)
		colour.colour = colourmap_get_colour(NULL, SC_ENC_RGBA, colour.colour);
	else if (colour.enc != SC_ENC_RGBA)
		return(0);
	return(RGB( (colour.colour & 0xff0000) >> 16, (colour.colour & 0xff00) >> 8, (colour.colour & 0xff) ));
}


