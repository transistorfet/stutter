/*
 * Module Name:		main.c
 * Version:		0.1
 * Module Requirements:	string
 * System Requirements:	Windows OS
 * Description:		Windows Frontend Main Loop
 */


#include <windows.h>

#include CONFIG_H
#include <stutter/key.h>
#include <stutter/init.h>
#include <stutter/type.h>
#include <stutter/signal.h>
#include <stutter/variable.h>
#include <stutter/lib/string.h>
#include <stutter/lib/globals.h>
#include <stutter/frontend/widget.h>
#include <stutter/frontend/common.h>

#include "net.h"
#include "terminal.h"

int handle_quit(char *, void *, char *);

DEFINE_TYPE_LIST(fe_types,
	ADD_TYPE(fe_common_load_colour)
	ADD_TYPE(fe_common_load_style)
	ADD_TYPE(fe_common_load_attrib)
	ADD_TYPE(fe_common_load_command)
);

DEFINE_HANDLER_LIST(fe_handlers,
	ADD_HANDLER("fe.quit", NULL, 0, handle_quit, NULL)
);

DEFINE_KEY_LIST(fe_keys,
	FE_BINDINGS()
);

HINSTANCE this_instance;
char *WinName = "Stutter";
struct variable_table_s *fe_table;

extern int init_execute(void);
extern int release_execute(void);

extern int init_timer(void);
extern int release_timer(void);

extern int init_frontend(void);
extern int release_frontend(void);

LRESULT CALLBACK windows_callback(HWND, UINT, WPARAM, LPARAM);

int init_windows(void)
{
	WNDCLASSEX winclass;
	struct type_s *type;

	winclass.cbSize = sizeof(WNDCLASSEX);
	winclass.hInstance = this_instance;
	winclass.lpszClassName = WinName;
	winclass.lpfnWndProc = windows_callback;
	winclass.cbClsExtra = 0;
	winclass.cbWndExtra = 0;

	winclass.style = 0;
	winclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	winclass.hIconSm = NULL;
	winclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	winclass.lpszMenuName = "StutterMenu";
	winclass.hbrBackground = NULL;

	if (!RegisterClassEx(&winclass))
		return(-1);
	if (init_system()) 
		return(-1);

	add_signal("fe.quit", 0);
	ADD_TYPE_LIST(fe_types);
	ADD_HANDLER_LIST(fe_handlers);

	if (init_net())
		return(-1);
	if (init_execute())
		return(-1);
	if (init_timer())
		return(-1);

	#undef MODULE
	#define MODULE(name)	LOAD_MODULE(name)
	MODULE_LIST()

	if (!(type = find_type("table")) || !(fe_table = add_variable(NULL, type, "fe", 0, "")))
		return(-1);
	add_variable(fe_table, type, "theme", 0, "");

	if (!(type = find_type("command:fe")))
		return(-1);
	add_variable(fe_table, type, "insert", 0, "callback,widget", fe_common_cmd_insert, NULL);

	if (!(type = find_type("string")))
		return(-1);
	add_variable(fe_table, type, "colour_test", 0, "string", "\x03\x30K0 \x03\x31K1 \x03\x32K2 \x03\x33K3 \x03\x34K4 \x03\x35K5 \x03\x36K6 \x03\x37K7 \x03\x38K8 \x03\x39K9 \x03\x31\x30K10 \x03\x31\x31K11 \x03\x31\x32K12 \x03\x31\x33K13 \x03\x31\x34K14 \x03\x31\x35K15");
	add_variable(fe_table, type, "bold_test", 0, "string", "\x02\x03\x30K0 \x03\x31K1 \x03\x32K2 \x03\x33K3 \x03\x34K4 \x03\x35K5 \x03\x36K6 \x03\x37K7 \x03\x38K8 \x03\x39K9 \x03\x31\x30K10 \x03\x31\x31K11 \x03\x31\x32K12 \x03\x31\x33K13 \x03\x31\x34K14 \x03\x31\x35K15");

	if (!(type = find_type("attrib:fe")))
		return(-1);
	add_variable(fe_table, type, "theme.bracket", 0, "string", FE_THEME_BRACKET);
	add_variable(fe_table, type, "theme.error", 0, "string", FE_THEME_ERROR);
	add_variable(fe_table, type, "theme.message", 0, "string", FE_THEME_MESSAGE);
	add_variable(fe_table, type, "theme.nick", 0, "string", FE_THEME_NICK);
	add_variable(fe_table, type, "theme.status", 0, "string", FE_THEME_STATUS);
	add_variable(fe_table, type, "theme.statusbar", 0, "string", FE_THEME_STATUSBAR);
	add_variable(fe_table, type, "theme.topic", 0, "string", FE_THEME_TOPIC);

	if (type = find_type("format"))
		add_variable(fe_table, type, "statusbar", 0, "string", FE_STATUSBAR_DEFAULT);
	ADD_KEY_LIST(fe_keys);

	if (init_terminal())
		return(-1);
	if (init_frontend()) 
		return(-1);
	return(0);
}

int release_windows(void)
{
	#undef MODULE
	#define MODULE(name)	RELEASE_MODULE(name)
	MODULE_LIST()

	release_terminal();
	release_frontend();
	release_timer();
	release_execute();
	release_net();
	release_system();
	return(0);
}

/**
 * Main Entry Point
 */
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR argv, int WinMode)
{
	MSG msg;

	/*** Initialization ***/
	this_instance = hInst;

	if (init_windows()) {
		MessageBox(NULL, "Failed to initialize system", NULL, MB_OK);
		release_windows();
		return(0);
	}

	#ifdef STUTTER_INIT
	STUTTER_INIT(argc, argv);
	#endif

	/*** Main Loop ***/
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	release_windows();
	return(msg.wParam);
}

/*** Local Functions ***/

int handle_quit(char *env, void *index, char *msg)
{
	PostQuitMessage(0);
	return(SIGNAL_STOP_EMIT);
}

LRESULT CALLBACK windows_callback(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message) {
		case WM_DESTROY: {
			struct terminal_s *terminal;

			if (terminal = terminal_find(hwnd))
				terminal_free(terminal);
			if (!terminal_get_number())
				fe_quit("Lost Terminal");
			break;
		}
		case NET_MESSAGE: {
			fe_net_handle_message(wparam, LOWORD(lparam), HIWORD(lparam));
			InvalidateRect(hwnd, NULL, 1);
			break;
		}
		case WM_KEYDOWN: {
			if ((wparam = terminal_convert_char(wparam)) == -1)
				break;
		}
		case WM_CHAR: {
			struct widget_s *input;

			if (process_key(wparam) && (input = (struct input_s *) fe_current_widget("input", NULL)))
				widget_control(input, WCC_PROCESS_CHAR, wparam);
			InvalidateRect(hwnd, NULL, 1);
			break;
		}
		case WM_SIZING: {
			struct terminal_s *terminal;

			if (terminal = terminal_find(hwnd)) {
				if (terminal_resizing(terminal, (RECT *) lparam, wparam))
					; // TODO cause a resize/update of widgets
				InvalidateRect(hwnd, NULL, 1);
			}
			break;
		}
		case WM_PAINT: {
			struct terminal_s *terminal;

			if (terminal = terminal_find(hwnd)) {
				fe_refresh();
				terminal_refresh(terminal);
			}
			break;
		}
		default:
			return(DefWindowProc(hwnd, message, wparam, lparam));
	}

	return(0);
}


