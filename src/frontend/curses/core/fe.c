/*
 * Module Name:		fe.c
 * Description:		Curses Frontend
 */

#include CONFIG_H
#include <stutter/debug.h>
#include <stutter/frontend/net.h>
#include <stutter/frontend/common.h>
#include <stutter/frontend/execute.h>
#include <stutter/frontend/frontend.h>
#include <stutter/frontend/curses/tcp.h>
#include <stutter/frontend/common/widget.h>
#include <stutter/frontend/common/surface.h>
#include <stutter/frontend/curses/terminal.h>
#include <stutter/frontend/curses/interface.h>
#include <stutter/frontend/common/widgets/page.h>

static struct fe_terminal *root_terminal = NULL;

int init_frontend(void)
{
	struct fe_widget *root;

	if (root_terminal)
		return(1);
	if (!(root_terminal = FE_TERMINAL(create_object(OBJECT_TYPE_S(&fe_terminal_type), ""))))
		return(-1);
	if (!(root = FE_WIDGET(create_object(OBJECT_TYPE_S(&fe_page_type), "s", "page"))))
		return(-1);
	fe_terminal_control(root_terminal, SCC_SET_ROOT, root, NULL);
	return(0);
}

int release_frontend(void)
{
	if (!root_terminal)
		return(1);
	destroy_object(OBJECT_S(root_terminal));
	root_terminal = NULL;
	return(0);
}

struct fe_widget *fe_get_focus(char *type)
{
	return(FE_PAGE(FE_SURFACE(root_terminal)->root)->input);
}

struct fe_widget *fe_get_target(struct fe_widget *widget, char *type)
{
	struct fe_widget *target;

	if (fe_widget_control(FE_PAGE(FE_SURFACE(root_terminal)->root)->frame, WCC_CURRENT_WIDGET, &target))
		return(NULL);
	return(target);
}


/*** Frontend Network Extension API */

/**
 * Create a new network connection and connect to the server:port given.
 */
fe_network_t fe_net_connect(const char *server, int port)
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
	return(fe_interface_set_callback(FE_INTERFACE(inter), condition, func, ptr));
}

/**
 * Send the string of length size to the given network connection and
 * return the number of bytes written or -1 on error.
 */
int fe_net_send(fe_network_t inter, const char *msg, int len)
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

	return(NULL);
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



