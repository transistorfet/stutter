/*
 * Module Name:		tcp.c
 * System Requirements:	Unix Sockets
 * Description:		TCP Network Interface Manager
 */


#include <string.h>
#include <stdarg.h>

#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>

#include <stutter/globals.h>

#include <stutter/object.h>
#include <stutter/frontend/curses/interface.h>
#include <stutter/frontend/curses/tcp.h>

#ifndef TCP_CONNECT_ATTEMPTS
#define TCP_CONNECT_ATTEMPTS		3
#endif

#ifndef TCP_LISTEN_QUEUE
#define TCP_LISTEN_QUEUE		5
#endif

struct fe_interface_type fe_tcp_type = { {
	NULL,
	"tcp",
	sizeof(struct fe_tcp),
	NULL,
	(object_init_t) fe_tcp_init,
	(object_release_t) fe_tcp_release },
	(fe_int_read_t) fe_tcp_read,
	(fe_int_write_t) fe_tcp_write
};

static int fe_tcp_init_connect(struct fe_tcp *, const char *, int);
static int fe_tcp_init_listen(struct fe_tcp *, int);
static int fe_tcp_init_accept(struct fe_tcp *, struct fe_tcp *);

int fe_tcp_init(struct fe_tcp *inter, const char *params, va_list va)
{
	int type;

	if (params[0] != 'n')
		return(-1);
	if (fe_interface_init(FE_INTERFACE(inter), 0, va) < 0)
		return(-1);

	type = va_arg(va, int);
	switch (type) {
		case FE_TCP_CONNECT: {
			const char *server;
			int port;

			server = va_arg(va, const char *);
			port = va_arg(va, int);
			return(fe_tcp_init_connect(inter, server, port));
		}
		case FE_TCP_LISTEN: {
			int port;

			port = va_arg(va, int);
			return(fe_tcp_init_listen(inter, port));
		}
		case FE_TCP_ACCEPT: {
			struct fe_tcp *listener;

			listener = va_arg(va, struct fe_tcp *);
			return(fe_tcp_init_accept(inter, listener));
			break;
		}
		default:
			break;
	}
	return(-1);
}

void fe_tcp_release(struct fe_tcp *inter)
{
	shutdown(FE_INTERFACE(inter)->read, 2);
	close(FE_INTERFACE(inter)->read);
	FE_INTERFACE(inter)->read = 0;
	fe_interface_release(FE_INTERFACE(inter));
}


int fe_tcp_read(struct fe_tcp *inter, char *str, int max)
{
	int res;

	if ((res = fe_tcp_receive(inter, str, max - 1)) < 0)
		return(res);
	str[res] = '\0';
	return(res);
}

int fe_tcp_write(struct fe_tcp *inter, const char *str, int len)
{
	if (len < 0)
		len = strlen(str);
	return(fe_tcp_send(inter, str, len));
}


/**
 * Receive the given number of bytes, store them in the given buffer
 * and return the number of bytes read or -1 on error or disconnect.
 */ 
int fe_tcp_receive(struct fe_tcp *inter, char *buffer, int size)
{
	int i, j;
	fd_set rd;
	struct timeval timeout = { 0, 0 };

	if (!inter)
		return(-1);
	FE_INTERFACE_SET_NOT_READY_READ(inter);

	for (i = 0; i < size; i++) {
		if (inter->read_pos >= inter->read_length)
			break;
		buffer[i] = inter->read_buffer[inter->read_pos++];
	}

	if (i < size) {
		FD_ZERO(&rd);
		FD_SET(FE_INTERFACE(inter)->read, &rd);
		if (select(FE_INTERFACE(inter)->read + 1, &rd, NULL, NULL, &timeout)
		    && ((j = recv(FE_INTERFACE(inter)->read, &buffer[i], size - i, 0)) > 0))
			i += j;
		if (j <= 0)
			return(-1);
	}
	if (inter->read_pos < inter->read_length)
		FE_INTERFACE_SET_READY_READ(inter);
	return(i);
}

/**
 * Send the string of length size to the given network connection and
 * return the number of bytes written or -1 on error.
 */
int fe_tcp_send(struct fe_tcp *inter, const char *data, int size)
{
	int sent, count = 0;

	if (!inter)
		return(0);
	do {
		if ((sent = send(FE_INTERFACE(inter)->read, (void *) data, size, 0)) < 0)
			return(-1);
		else if (!sent)
			return(0);
		count += sent;
	} while (count < size);
	return(count);
}


/**
 * Receive data directly into the read buffer and return the number of bytes
 * read or -1 on error or disconnect.
 */ 
int fe_tcp_read_to_buffer(struct fe_tcp *inter)
{
	int res;
	fd_set rd;
	struct timeval timeout = { 0, 0 };

	FD_ZERO(&rd);
	FD_SET(FE_INTERFACE(inter)->read, &rd);
	if ((res = select(FE_INTERFACE(inter)->read + 1, &rd, NULL, NULL, &timeout)) < 0)
		return(-1);
	else if (res == 0)
		return(inter->read_length - inter->read_pos);

	if ((res = recv(FE_INTERFACE(inter)->read, &inter->read_buffer[inter->read_length], TCP_READ_BUFFER - inter->read_length - 1, 0)) <= 0)
		return(-1);
	res += inter->read_length;
	inter->read_length = res;
	inter->read_buffer[res] = '\0';
	FE_INTERFACE_SET_READY_READ(inter);
	return(res - inter->read_pos);
}

/**
 * Set the read buffer to contain the given data.
 */
int fe_tcp_set_read_buffer(struct fe_tcp *inter, const char *buffer, int len)
{
	if (len > TCP_READ_BUFFER)
		len = TCP_READ_BUFFER;
	strncpy(inter->read_buffer, buffer, len);
	inter->read_length = len;
	inter->read_pos = 0;
	if (len)
		FE_INTERFACE_SET_READY_READ(inter);
	else
		FE_INTERFACE_SET_NOT_READY_READ(inter);
	return(len);
}

/**
 * Set the position in the read buffer that has been read to the given value or
 * if the position is greater than or equal to the length of the read buffer,
 * clear the read buffer.  The position of the read pointer is returned.
 */
int fe_tcp_advance_read_position(struct fe_tcp *inter, int pos)
{
	if (pos >= inter->read_length) {
		FE_INTERFACE_SET_NOT_READY_READ(inter);
		inter->read_pos = 0;
		inter->read_length = 0;
	}
	else {
		FE_INTERFACE_SET_READY_READ(inter);
		inter->read_pos = pos;
	}
	return(inter->read_pos);
}

/**
 * Clear the read buffer.
 */
void fe_tcp_clear_buffer(struct fe_tcp *inter)
{
	FE_INTERFACE_SET_NOT_READY_READ(inter);
	inter->read_pos = 0;
	inter->read_length = 0;
}


/*** Local Functions ***/

static int fe_tcp_init_connect(struct fe_tcp *inter, const char *server, int port)
{
	int i, j;
	struct hostent *host;
	struct sockaddr_in saddr;

	if (!(host = gethostbyname(server)))
		return(-1);
	memset(&saddr, '\0', sizeof(struct sockaddr_in));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);

	if ((FE_INTERFACE(inter)->read = socket(PF_INET, SOCK_STREAM, 0)) >= 0) {
		for (j = 0; host->h_addr_list[j]; j++) {
			saddr.sin_addr = *((struct in_addr *) host->h_addr_list[j]);
			for (i = 0; i < TCP_CONNECT_ATTEMPTS; i++) {
				if (connect(FE_INTERFACE(inter)->read, (struct sockaddr *) &saddr, sizeof(struct sockaddr_in)) >= 0)
					return(0);
			}
		}
	}
	return(-1);
}

static int fe_tcp_init_listen(struct fe_tcp *inter, int port)
{
	struct sockaddr_in saddr;

	memset(&saddr, '\0', sizeof(struct sockaddr_in));
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = INADDR_ANY;
	saddr.sin_port = htons(port);

	if (((FE_INTERFACE(inter)->read = socket(PF_INET, SOCK_STREAM, 0)) >= 0)
	    && (bind(FE_INTERFACE(inter)->read, (struct sockaddr *) &saddr, sizeof(struct sockaddr_in)) >= 0)
	    && (listen(FE_INTERFACE(inter)->read, TCP_LISTEN_QUEUE) >= 0)) {
		return(0);
	}
	return(-1);
}

static int fe_tcp_init_accept(struct fe_tcp *inter, struct fe_tcp *listener)
{
	int size;
	fd_set rd;
	struct sockaddr_in saddr;
	struct timeval timeout = { 0, 0 };

	/** Make sure there is a connection waiting */
	FD_ZERO(&rd);
	FD_SET(FE_INTERFACE(listener)->read, &rd);
	if (select(FE_INTERFACE(listener)->read + 1, &rd, NULL, NULL, &timeout) <= 0)
		return(-1);

	size = sizeof(struct sockaddr_in);
	if ((FE_INTERFACE(inter)->read = accept(FE_INTERFACE(listener)->read, (struct sockaddr *) &saddr, &size))) {
		//fe_status("Accepted connection from %s", inet_ntoa(saddr.sin_addr));
		// TODO do anything you might want with the saddr
		return(0);
	}
	return(-1);
}


