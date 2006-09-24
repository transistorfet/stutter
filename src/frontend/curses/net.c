/*
 * Module Name:		net.c
 * Version:		0.1
 * Module Requirements:	debug
 * System Requirements:	Unix Sockets
 * Description:		Network Interface Manager
 */


#include <string.h>

#include <netdb.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>

#include <stutter/lib/debug.h>
#include <stutter/lib/memory.h>
#include "net.h"
#include "desc.h"

static struct fe_descriptor_list_s *net_list;

static void fe_net_free_socket(struct fe_descriptor_s *);

int init_net(void)
{
	if (net_list)
		return(1);
	if (!(net_list = fe_desc_create_list((destroy_t) fe_net_free_socket)))
		return(-1);
	return(0);
}

int release_net(void)
{
	if (!net_list)
		return(1);
	fe_desc_destroy_list(net_list);
	return(0);
}

/**
 * Create a new network connection and connect to the server:port given.
 */
fe_network_t fe_net_connect(char *server, int port, callback_t receiver, void *ptr)
{
	int i, j;
	struct hostent *host;
	struct sockaddr_in saddr;
	struct fe_descriptor_s *desc;

	if (!(host = gethostbyname(server)))
		return(NULL);
	memset(&saddr, '\0', sizeof(struct sockaddr_in));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);

	if (!(desc = fe_desc_create(net_list, 0)))
		return(NULL);
	if ((desc->read = socket(AF_INET, SOCK_STREAM, 0)) >= 0) {
		for (j = 0;host->h_addr_list[j];j++) {
			saddr.sin_addr = *((struct in_addr *) host->h_addr_list[j]);
			for (i = 0;i < NET_ATTEMPTS;i++) {
				if (connect(desc->read, (struct sockaddr *) &saddr, sizeof(struct sockaddr_in)) >= 0) {
					signal_connect("fe.read_ready", desc, 10, receiver, ptr);
					return(desc);
				}
			}
		}
	}
	fe_desc_destroy(net_list, desc);
	return(NULL);
}

/**
 * Listen on the given port of connections and call the recevier callback
 * with the fe_network_t of the connection and the given ptr.  The fe_network_t
 * associated with the server is returned or NULL on error.
 */
fe_network_t fe_net_listen(int port, callback_t receiver, void *ptr)
{
	// TODO add server capabilities
}

/**
 * Set the receiver callback of the given network connection to the given
 * callback and ptr.  If the given net is valid then 0 is returned
 * otherwise -1 is returned.
 */
int fe_net_set_receiver(fe_network_t desc, callback_t receiver, void *ptr)
{
	if (!desc)
		return(-1);
	// TODO ???
	return(0);
}

/**
 * Disconnect the given network connection.
 */
void fe_net_disconnect(fe_network_t desc)
{
	if (!desc)
		return;
	fe_desc_destroy(net_list, desc);
}

/**
 * Send the string of length size to the given network connection and
 * return the number of bytes written or -1 on error.
 */
int fe_net_send(fe_network_t desc, char *msg, int size)
{
	int sent, count = 0;

	if (!desc)
		return(0);
	DEBUG_LOG("raw.out", msg);
	do {
		if ((sent = send(desc->read, (void *) msg, size, 0)) < 0)
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
int fe_net_receive(fe_network_t desc, char *msg, int size)
{
	int i, j;
	fd_set rd;
	struct timeval timeout = { 0, 0 };

	if (!desc)
		return(-1);

	size--;
	for (i = 0;i < size;i++) {
		if (desc->read_pos >= desc->read_length)
			break;
		msg[i] = desc->read_buffer[desc->read_pos++];
	}

	if (i < size) {
		FD_ZERO(&rd);
		FD_SET(desc->read, &rd);
		if (select(desc->read + 1, &rd, NULL, NULL, &timeout) && ((j = recv(desc->read, &msg[i], size - i, 0)) > 0))
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
int fe_net_receive_str(fe_network_t desc, char *msg, int size, char ch)
{
	int i;
	fd_set rd;
	struct timeval timeout = { 0, 0 };

	if (!desc)
		return(-1);

	size--;
	for (i = 0;i < size;i++) {
		if (desc->read_pos >= desc->read_length) {
			FD_ZERO(&rd);
			FD_SET(desc->read, &rd);
			// TODO if either of these fail, we return a partial message which the server
			//	doesn't check for and can't deal with so what we really need to do is
			//	copy what we've read (in msg) to the buffer and return 0 but what
			//	if the msg is bigger than the buffer?
			// TODO what about a socket error of some sorts too.  That doesn't really count
			//	as a msg that hasn't been fully recieved.  We could send a signal to report
			//	the error somehow (which raises the question of should we do (and how would
			//	we do) socket specific signals.
			if (!select(desc->read + 1, &rd, NULL, NULL, &timeout)) {
				msg[i + 1] = '\0';
				return(i);
			}
			if ((desc->read_length = recv(desc->read, desc->read_buffer, DESC_READ_BUFFER, 0)) <= 0)
				return(-1);
			desc->read_pos = 0;
		}
		msg[i] = desc->read_buffer[desc->read_pos++];
		if (msg[i] == ch)
			break;
	}

	msg[i + 1] = '\0';
	DEBUG_LOG("raw.in", msg);
	return(i);
}

/*** Local Functions ***/

static void fe_net_free_socket(struct fe_descriptor_s *desc)
{
	shutdown(desc->read, 2);
	close(desc->read);
}


