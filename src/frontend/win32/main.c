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

#include "net.h"
#include "terminal.h"

HINSTANCE this_instance;
char *WinName = "Stutter";
struct variable_table_s *fe_table;

extern int init_frontend(void);
extern int release_frontend(void);

int handle_quit(char *, void *, char *);
LRESULT CALLBACK windows_callback(HWND, UINT, WPARAM, LPARAM);

int init_windows(void)
{
	WNDCLASSEX winclass;
	struct type_s *type;
	struct variable_s *var;

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
	if (init_net())
		return(-1);

	add_signal("quit", 0);
	signal_connect("quit", NULL, 0, (signal_t) handle_quit, NULL);

	LOAD_MODULES();
	if (!(type = find_type("table")) || !(var = add_variable(NULL, type, "fe", 0, "")))
		return(-1);
	fe_table = var->value;

	if (!(type = find_type("string")))
		return(-1);
	add_variable(fe_table, type, "bold", 0, "");
	add_variable(fe_table, type, "normal", 0, "");
	add_variable(fe_table, type, "flash", 0, "");
	add_variable(fe_table, type, "inverse", 0, "\x16");

	if (type = find_type("format"))
		add_variable(NULL, type, "fe.status", 0, "%s", FE_STATUS_BAR);
	BIND_KEYS();

	if (init_terminal())
		return(-1);
	if (init_frontend()) 
		return(-1);
	return(0);
}

int release_windows(void)
{
	release_terminal();
	release_frontend();
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
	//STUTTER_INIT(argc, argv);
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
		case NET_MESSAGE:
			fe_net_handle_message(wparam, LOWORD(lparam), HIWORD(lparam));
			InvalidateRect(hwnd, NULL, 1);
			break;
		case WM_KEYDOWN:
			if ((wparam = terminal_convert_char(wparam)) == -1)
				break;
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
				terminal_resizing(terminal, (RECT *) lparam, wparam);
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


