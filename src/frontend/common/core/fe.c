/*
 * Module Name:		fe.c
 * Description:		Curses Frontend
 */


#include CONFIG_H
#include <stutter/frontend/common.h>
#include <stutter/frontend/common/api.h>
#include <stutter/frontend/common/widget.h>
#include <stutter/frontend/common/surface.h>

extern int init_terminal(void);
extern int release_terminal(void);

static int add_builtin_layouts(void);

int init_frontend(void)
{
	if (init_fe_common_api())
		return(-1);
	if (init_terminal())
		return(-1);
	add_builtin_layouts();

	if (!(fe_create_widget("fe", "root", "root", NULL)))
		return(-1);
	return(0);
}

int release_frontend(void)
{
	release_terminal();
	release_fe_common_api();
	return(0);
}

void *fe_create_widget(char *ns, char *type, char *id, void *parent)
{
	return(fe_common_api_create_widget(ns, type, id, parent));
}

int fe_destroy_widget(void *widget)
{
	return(fe_common_api_destroy_widget(widget));
}

void *fe_get_parent(void *widget)
{
	return(fe_common_api_get_parent(widget));
}

short fe_get_width(void *widget)
{
	return(fe_common_api_get_width(widget));
}

short fe_get_height(void *widget)
{
	return(fe_common_api_get_height(widget));
}

void *fe_find_widget(char *id)
{
	return(fe_common_api_find_widget(id));
}

int fe_resize_widget(void *widget, int x, int y)
{
	return(fe_common_api_resize_widget(widget, x, y));
}


void *fe_get_focus(char *type)
{
	return(fe_common_api_get_focus(type));
}

int fe_set_focus(void *widget)
{
	return(fe_common_api_set_focus(widget));
}

void *fe_get_target(void *widget, char *type)
{
	return(fe_common_api_get_target(widget, type));
}

int fe_set_target(void *widget)
{
	return(fe_common_api_set_target(widget));
}


int fe_show_widget(void *widget)
{
	return(fe_common_api_show_widget(widget));
}


int fe_print(void *widget, const char *str)
{
	return(fe_common_api_print(widget, str));
}

int fe_read(void *widget, char *buffer, int max)
{
	return(fe_common_api_read(widget, buffer, max));
}

void fe_clear(void *widget)
{
	fe_common_api_clear(widget);
}

void fe_move(void *widget, short x, short y)
{
	fe_common_api_move(widget, x, y);
}

int fe_scroll(void *widget, int diff)
{
	return(fe_common_api_scroll(widget, diff));
}

void fe_quit(char *reason)
{
	emit_signal(NULL, "fe.quit", (void *) reason);
}

int fe_refresh(void)
{
	return(fe_common_api_refresh());
}

/*** Frontend Network Extension API */

/**
 * Create a new network connection and connect to the server:port given.
 */
fe_network_t fe_net_connect(char *server, int port)
{
	return(fe_tcp_connect(server, port));
}

/**
 * Listen on the given port of connections and call the recevier callback
 * with the fe_network_t of the connection and the given ptr.  The fe_network_t
 * associated with the server is returned or NULL on error.
 */
fe_network_t fe_net_listen(int port)
{
	// TODO add server capabilities
	return(NULL);
}

/**
 * Disconnect the given network connection.
 */
void fe_net_disconnect(fe_network_t inter)
{
	destroy_object(OBJECT_S(inter));
}

/**
 * Returns the callback for the given process.
 */
struct callback_s fe_net_get_callback(fe_network_t inter)
{
	return(fe_interface_get_callback(FE_INTERFACE(inter)));
}

/**
 * Sets the callback for the given process to be executed under the given
 * conditions.
 */
void fe_net_set_callback(fe_network_t inter, int condition, callback_t func, void *ptr)
{
	return(fe_interface_set_callback(FE_INTERFACE(inter), cond, func, ptr));
}

/**
 * Send the string of length size to the given network connection and
 * return the number of bytes written or -1 on error.
 */
int fe_net_send(fe_network_t inter, char *msg, int len)
{
	DEBUG_LOG("raw.out", "%s", msg);
	return(FE_INTERFACE_WRITE(FE_INTERFACE(inter), msg, len));
}

/**
 * Receive the given number of bytes, store them in the given msg buffer
 * and return the number of bytes read or -1 on error or disconnect.
 */ 
int fe_net_receive(fe_network_t inter, char *buffer, int size)
{
	int ret;

	if ((ret = FE_INTERFACE_READ(FE_INTERFACE(inter), buffer, size)))
		DEBUG_LOG("raw.in", "%s", buffer);
	return(ret);
}


/*** Frontend Execute Extension API */

/**
 * Execute the given command and return a reference to the running program.
 */
fe_execute_t fe_execute_open(char *cmd, int bitflags)
{

}

/**
 * Close the given program reference.
 */
void fe_execute_close(fe_execute_t inter)
{

}

/**
 * Returns the callback for the given process.
 */
struct callback_s fe_execute_get_callback(fe_execute_t inter)
{

}

/**
 * Sets the callback for the given process to be executed under the given
 * conditions.
 */
void fe_execute_set_callback(fe_execute_t inter, int condition, callback_t func, void *ptr)
{

}


/**
 * Send the string of length len to the given process and
 * return the number of bytes written or -1 on error.
 */
int fe_execute_send(fe_execute_t inter, char *buffer, int len)
{

}

/**
 * Receive the given number of bytes, store them in the given buffer
 * and return the number of bytes read or -1 on error or disconnect.
 */ 
int fe_execute_receive(fe_execute_t inter, char *buffer, int len)
{

}


/*** Local Functions ***/

// TODO this is here until you can decide on how layouts will be created
static int add_builtin_layouts(void)
{
	struct layout_s *layout;

	layout = make_layout("frame", make_layout_property("id", "frame", NULL), NULL, NULL);
	layout->next = make_layout("statusbar", make_layout_property("id", "statusbar", make_layout_property("text", FE_DEFAULT_STATUSBAR, NULL)), NULL, NULL);
	layout->next->next = make_layout("input", make_layout_property("id", "input", NULL), NULL, NULL);

	add_layout("", "root", make_layout("terminal", NULL, make_layout("region", make_layout_property("id", "region", make_layout_property("width", "80", make_layout_property("height", "25", NULL))), layout, NULL), NULL));
	add_layout("", "text", make_layout("text", NULL, NULL, NULL));
	return(0);
}


