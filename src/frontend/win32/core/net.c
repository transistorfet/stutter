/*
 * Module Name:		net.c
 * Version:		0.1
 * Module Requirements:	debug
 * System Requirements:	Windows OS ; Winsock
 * Description:		Windows Network Interface Manager
 */


#include <string.h>
#include <windows.h>
#include <winsock.h>

#include CONFIG_H
#include <stutter/debug.h>
#include <stutter/memory.h>
#include <stutter/signal.h>
#include "net.h"

#ifndef NET_READ_BUFFER
#define NET_READ_BUFFER		512
#endif

struct fe_network_s {
	int socket;
	int read;
	int length;
	char buffer[NET_READ_BUFFER];
	struct fe_network_s *next;
};

extern HINSTANCE this_instance;

static HWND net_hwnd;
static fe_network_t net_list = NULL;

extern int fe_refresh(void);
LRESULT CALLBACK net_callback(HWND, UINT, WPARAM, LPARAM);

int init_net(void)
{
	WSADATA tmp;
	WNDCLASSEX winclass;

	if (WSAStartup(0x0101, &tmp))
		return(-1);

	winclass.cbSize = sizeof(WNDCLASSEX);
	winclass.hInstance = this_instance;
	winclass.lpszClassName = "net";
	winclass.lpfnWndProc = net_callback;
	winclass.cbClsExtra = 0;
	winclass.cbWndExtra = 0;

	winclass.style = 0;
	winclass.hIcon = NULL;
	winclass.hIconSm = NULL;
	winclass.hCursor = NULL;
	winclass.lpszMenuName = NULL;
	winclass.hbrBackground = NULL;

	if (!RegisterClassEx(&winclass))
		return(-1);
	net_hwnd = CreateWindow("net", "net", WS_POPUP, 0, 0, 0, 0, NULL, NULL, this_instance, NULL);
	add_signal("fe.read_ready", 0);
	return(0);
}

int release_net(void)
{
	fe_network_t cur, tmp;

	cur = net_list;
	while (cur) {
		tmp = cur->next;
		shutdown(cur->socket, 2);
		close(cur->socket);
		memory_free(cur);
		cur = tmp;
	}

	remove_signal("fe.read_ready");
	DestroyWindow(net_hwnd);
	WSACleanup();
	return(0);
}

/**
 * Create a new network connection and connect to the server:port given.
 */
fe_network_t fe_net_connect(char *server, int port)
{
	int i, j;
	int sockfd;
	fe_network_t net;
	struct hostent *host;
	struct sockaddr_in saddr;

	if (!(net = (fe_network_t) memory_alloc(sizeof(struct fe_network_s)))) {
		FE_NET_ERROR_JOINPOINT(FE_NET_ERR_FAILED_TO_CONNECT, server)
		return(NULL);
	}
	memset(net, '\0', sizeof(struct fe_network_s));

	// TODO call inet_addr first
	if (!(host = gethostbyname(server))) {
		FE_NET_ERROR_JOINPOINT(FE_NET_ERR_FAILED_TO_CONNECT, server)
		return(NULL);
	}
	memset(&saddr, '\0', sizeof(struct sockaddr_in));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);

	if ((net->socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		FE_NET_ERROR_JOINPOINT(FE_NET_ERR_FAILED_TO_CONNECT, server)
		return(NULL);
	}
	for (j = 0;host->h_addr_list[j];j++) {
		saddr.sin_addr = *((struct in_addr *) host->h_addr_list[j]);
		for (i = 0;i < NET_ATTEMPTS;i++) {
			if (connect(net->socket, (struct sockaddr *) &saddr, sizeof(struct sockaddr_in)) >= 0) {
				WSAAsyncSelect(net->socket, net_hwnd, NET_MESSAGE, FD_READ | FD_CLOSE);
				net->next = net_list;
				net_list = net;
				return(net);
			}
			FE_NET_ERROR_JOINPOINT(FE_NET_ERR_RETRYING, server)
		}
	}
	FE_NET_ERROR_JOINPOINT(FE_NET_ERR_FAILED_TO_CONNECT, server)
	return(NULL);
}

/**
 * Listen on the given port of connections and call the recevier callback
 * with the fe_network_t of the connection and the given ptr.  The fe_network_t
 * associated with the server is returned or NULL on error.
 */
fe_network_t fe_net_listen(int port)
{
	// TODO add server capabilities
}

/**
 * Disconnect the given network connection.
 */
void fe_net_disconnect(fe_network_t net)
{
	int i;
	fe_network_t cur, prev;

	if (!net)
		return;

	cur = net_list;
	prev = NULL;
	while (cur) {
		if (cur == net) {
			if (prev)
				prev->next = cur->next;
			else
				net_list = cur->next;
			break;
		}
		prev = cur;
		cur = cur->next;
	}

	shutdown(net->socket, 2);
	close(net->socket);
	memory_free(net);
}

/**
 * Send the string of length size to the given network connection and
 * return the number of bytes written or -1 on error.
 */
int fe_net_send(fe_network_t net, char *msg, int size)
{
	int sent, count = 0;

	if (!net)
		return(0);
	DEBUG_LOG("raw.out", msg);
	do {
		if ((sent = send(net->socket, (void *) msg, size, 0)) < 0)
			return(-1);
		else if (!sent)
			return(0);
		count += sent;
	} while (count < size);
	return(count);
}

/**
 * Receive the given number of bytes, store them in the given msg buffer
 * and return the number of bytes read or -1 on error or disconnect.
 */ 
int fe_net_receive(fe_network_t net, char *msg, int size)
{
	int i, j;
	fd_set rd;
	struct timeval timeout = { 0, 0 };

	if (!net)
		return(-1);

	size--;
	for (i = 0;i < size;i++) {
		if (net->read >= net->length)
			break;
		msg[i] = net->buffer[net->read++];
	}

	if (i < size) {
		FD_ZERO(&rd);
		FD_SET(net->socket, &rd);
		if (select(net->socket + 1, &rd, NULL, NULL, &timeout) && ((j = recv(net->socket, &msg[i], size - i, 0)) > 0))
			i += j;
		if (j <= 0)
			return(-1);
	}

	msg[i] = '\0';
	DEBUG_LOG("raw.in", msg);
	return(i);
}

/**
 * Receive a string from the network connection up to a maximum of
 * size-1 (a null char is appended) and return the number of bytes
 * read or -1 on error or disconnect.
 */ 
int fe_net_receive_str(fe_network_t net, char *msg, int size, char ch)
{
	int i;
	fd_set rd;
	struct timeval timeout = { 0, 0 };

	if (!net)
		return(-1);

	size--;
	for (i = 0;i < size;i++) {
		if (net->read >= net->length) {
			FD_ZERO(&rd);
			FD_SET(net->socket, &rd);
			// TODO if either of these fail, we return a partial message which the server
			//	doesn't check for and can't deal with so what we really need to do is
			//	copy what we've read (in msg) to the buffer and return 0 but what
			//	if the msg is bigger than the buffer?
			// TODO what about a socket error of some sorts too.  That doesn't really count
			//	as a msg that hasn't been fully recieved.  We could send a signal to report
			//	the error somehow (which raises the question of should we do (and how would
			//	we do) socket specific signals.
			if (!select(net->socket + 1, &rd, NULL, NULL, &timeout)) {
				msg[i + 1] = '\0';
				return(i);
			}
			if ((net->length = recv(net->socket, net->buffer, NET_READ_BUFFER, 0)) <= 0)
				return(-1);
			net->read = 0;
		}
		msg[i] = net->buffer[net->read++];
		if (msg[i] == ch)
			break;
	}

	msg[i + 1] = '\0';
	DEBUG_LOG("raw.in", msg);
	return(i);
}

/**
 *
 */
int fe_net_handle_message(int socket, int condition, int error)
{
	fd_set rd;
	fe_network_t cur;
	struct timeval timeout = { 0, 0 };

	cur = net_list;
	while (cur) {
		if (cur->socket == socket) {
			// TODO is there a winsock specific call do check for data? (or just doing a
			//	nonblocking call???
			FD_ZERO(&rd);
			FD_SET(socket, &rd);
			while ((cur->read < cur->length) || select(socket + 1, &rd, NULL, NULL, &timeout))
				emit_signal("fe.read_ready", cur, cur);
			return(0);
		}
		cur = cur->next;
	}
	return(-1);
}

LRESULT CALLBACK net_callback(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message) {
		case NET_MESSAGE: {
			fe_net_handle_message(wparam, LOWORD(lparam), HIWORD(lparam));
			fe_refresh();
			break;
		}
		default:
			return(DefWindowProc(hwnd, message, wparam, lparam));
	}
	return(0);
}

