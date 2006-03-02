/*
 * Module Name:		net.c
 * Version:		0.1
 * Module Requirements:	debug
 * System Requirements:	Unix Sockets
 * Description:		Network Interface Manager
 */


#include <string.h>

#include <netdb.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>

#include <stutter/lib/debug.h>
#include <stutter/lib/memory.h>
#include "net.h"

struct network_s {
	int socket;
	callback_t receiver;
	void *ptr;
	struct network_s *next;
};

static network_t net_list = NULL;

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
	net->receiver = receiver;
	net->ptr = ptr;

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

	shutdown(net->socket, SHUT_RDWR);
	close(net->socket);
	memory_free(net);
}

/**
 * Send the string of length size to the given network connection and
 * return the number of bytes written or -1 on error.
 */
int fe_net_send(network_t net, char *msg, int size)
{
	if (!net)
		return(0);
	DEBUG_LOG("raw.out", msg);
	return(send(net->socket, (void *) msg, size, 0));
}

/**
 * Receive a string from the network connection up to a maximum of
 * size-1 (a null char is appended) and return the number of bytes
 * read or -1 on error.
 */ 
int fe_net_receive(network_t net, char *msg, int size)
{
	int i;
	fd_set rd;
	struct timeval timeout = { 0, 0 };

	if (!net)
		return(0);
	FD_ZERO(&rd);
	FD_SET(net->socket, &rd);
	if (!select(net->socket + 1, &rd, NULL, NULL, &timeout))
		return(0);

	for (i = 0;i < size;i++) {
		if ((recv(net->socket, (void *) &msg[i], 1, 0)) <= 0)
			return(-1);
		else if (msg[i] == '\n')
			break;
	}

	//if ((size = recv(net->socket, msg, size, 0)) <= 0)
	//	return(-1);
	msg[++i] = '\0';
	DEBUG_LOG("raw.in", msg);
	return(i);
}

/**
 * Wait for input on all sockets and STDIN for the time given in seconds and return
 * 0 if the time expires or the number of sockets with input.
 */
int fe_net_wait(float t)
{
	fd_set rd;
	int max, ret;
	network_t cur;
	struct timeval timeout;

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

	ret = select(max + 1, &rd, NULL, NULL, &timeout);
	if (ret == -1) {
		// TODO what do we do in the case of a socket error?
	}
	else {
		for (cur = net_list;cur;cur = cur->next) {
			if (cur->receiver && FD_ISSET(cur->socket, &rd))
				cur->receiver(cur->ptr, cur);
		}
	}

	return(ret);
}


