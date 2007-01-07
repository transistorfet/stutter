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
#include <stutter/string.h>
#include <stutter/globals.h>
#include <stutter/variable.h>
#include <stutter/frontend/widget.h>
#include <stutter/frontend/common.h>

#include "core/net.h"
#include "core/terminal.h"

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

DEFINE_VARIABLE_LIST(fe_variables,
	DECLARE_TYPE("attrib:fe",
		FE_COMMON_ATTRIBS()
	)
	DECLARE_TYPE("command:fe",
		FE_COMMON_COMMANDS()
	)
	DECLARE_TYPE("format",
		FE_COMMON_FORMATS()
	)
);

DEFINE_COMMAND_LIST(fe_commands,
	STUTTER_INIT_COMMANDS()
);

HINSTANCE this_instance;
char *WinName = "Stutter";
struct variable_table_s *fe_table;
struct variable_table_s *fe_theme;

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
	ADD_TYPE_LIST(fe_types)
	ADD_HANDLER_LIST(fe_handlers)

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
	fe_theme = add_variable(fe_table, type, "theme", 0, "");

	ADD_VARIABLE_LIST(fe_table, fe_variables)
	ADD_KEY_LIST(fe_keys)

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

	EVALUATE_COMMAND_LIST(fe_commands)

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


