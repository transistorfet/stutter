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

#include <stutter/lib/debug.h>
#include <stutter/lib/memory.h>
#include "net.h"
#include "terminal.h"

#ifndef NET_READ_BUFFER
#define NET_READ_BUFFER		512
#endif

struct network_s {
	int socket;
	callback_t receiver;
	void *ptr;
	int read;
	int length;
	char buffer[NET_READ_BUFFER];
	struct network_s *next;
};

static network_t net_list = NULL;

int init_net(void)
{
	WSADATA tmp;

	if (WSAStartup(0x0101, &tmp))
		return(-1);
	return(0);
}

int release_net(void)
{
	network_t cur, tmp;

	cur = net_list;
	while (cur) {
		tmp = cur->next;
		shutdown(cur->socket, 2);
		close(cur->socket);
		memory_free(cur);
		cur = tmp;
	}

	WSACleanup();
	return(0);
}

/**
 * Create a new network connection and connect to the server:port given.
 */
network_t fe_net_connect(char *server, int port, callback_t receiver, void *ptr)
{
	int i, j;
	int sockfd;
	network_t net;
	struct hostent *host;
	struct sockaddr_in saddr;

	if (!(net = (network_t) memory_alloc(sizeof(struct network_s))))
		return(NULL);
	memset(net, '\0', sizeof(struct network_s));
	net->receiver = receiver;
	net->ptr = ptr;

	// TODO call inet_addr first
	if (!(host = gethostbyname(server)))
		return(NULL);
	memset(&saddr, '\0', sizeof(struct sockaddr_in));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);

	if ((net->socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return(NULL);
	for (j = 0;host->h_addr_list[j];j++) {
		saddr.sin_addr = *((struct in_addr *) host->h_addr_list[j]);
		for (i = 0;i < NET_ATTEMPTS;i++) {
			if (connect(net->socket, (struct sockaddr *) &saddr, sizeof(struct sockaddr_in)) >= 0) {
				WSAAsyncSelect(net->socket, terminal_get_window(terminal), NET_MESSAGE, FD_READ | FD_CLOSE);
				net->next = net_list;
				net_list = net;
				return(net);
			}
		}
	}
	return(NULL);
}

/**
 * Listen on the given port of connections and call the recevier callback
 * with the network_t of the connection and the given ptr.  The network_t
 * associated with the server is returned or NULL on error.
 */
network_t fe_net_listen(int port, callback_t receiver, void *ptr)
{
	// TODO add server capabilities
}

/**
 * Set the receiver callback of the given network connection to the given
 * callback and ptr.  If the given net is valid then 0 is returned
 * otherwise -1 is returned.
 */
int fe_net_set_receiver(network_t net, callback_t receiver, void *ptr)
{
	if (!net)
		return(-1);
	net->receiver = receiver;
	net->ptr = ptr;
	return(0);
}

/**
 * Disconnect the given network connection.
 */
void fe_net_disconnect(network_t net)
{
	int i;
	network_t cur, prev;

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
int fe_net_send(network_t net, char *msg, int size)
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
int fe_net_receive(network_t net, char *msg, int size)
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
int fe_net_receive_str(network_t net, char *msg, int size, char ch)
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
	network_t cur;
	struct timeval timeout = { 0, 0 };

	cur = net_list;
	while (cur) {
		if (cur->socket == socket) {
			// TODO is there a winsock specific call do check for data? (or just doing a
			//	nonblocking call???
			FD_ZERO(&rd);
			FD_SET(socket, &rd);
			while ((cur->read < cur->length) || select(socket + 1, &rd, NULL, NULL, &timeout))
				cur->receiver(cur->ptr, cur);
			return(0);
		}
		cur = cur->next;
	}
	return(-1);
}

/**
 * Wait for input on all sockets and STDIN for the time given in seconds and return
 * 0 if the time expires or the number of sockets that had input.
 */
int fe_net_wait(float t)
{
	fd_set rd;
	network_t cur;
	int max, ret = 0;
	struct timeval timeout;

	/** Check the buffer of each connection to see if any messages are waiting
	    and return when each connection gets a chance to read one message so that
	    we can refresh the screen and check for keyboard input to remain responsive */
	for (cur = net_list;cur;cur = cur->next) {
		if (cur->read < cur->length) {
			cur->receiver(cur->ptr, cur);
			ret++;
		}
	}
	if (ret)
		return(ret);

	/** Check each connection's socket for input using select */
	timeout.tv_sec = (int) t;
	timeout.tv_usec = (int) ((t - timeout.tv_sec) * 1000000);

	FD_ZERO(&rd);
	FD_SET(0, &rd);
	max = 0;
	for (cur = net_list;cur;cur = cur->next) {
		FD_SET(cur->socket, &rd);
		if (cur->socket > max)
			max = cur->socket;
	}

	if ((ret = select(max + 1, &rd, NULL, NULL, &timeout)) == -1) {
		// TODO what do we do in the case of a socket error?
		return(-1);
	}

	for (cur = net_list;cur;cur = cur->next) {
		if ((cur->read < cur->length) || (cur->receiver && FD_ISSET(cur->socket, &rd)))
			cur->receiver(cur->ptr, cur);
	}
	return(ret);
}


