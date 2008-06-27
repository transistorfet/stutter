/*
 * Header Name:		tcp.h
 * Description:		TCP Network Interface Manager Header
 */

#ifndef _STUTTER_FRONTEND_CURSES_TCP_H
#define _STUTTER_FRONTEND_CURSES_TCP_H

#include <stdarg.h>

#include <stutter/object.h>
#include <stutter/globals.h>
#include <stutter/frontend/curses/interface.h>

#define FE_TCP_CONNECT		1
#define FE_TCP_LISTEN		2
#define FE_TCP_ACCEPT		3

#ifndef TCP_READ_BUFFER
#define TCP_READ_BUFFER		512
#endif

#define FE_TCP(ptr)		( (struct fe_tcp *) (ptr) )

struct fe_tcp {
	struct fe_interface interface;
	int read_pos;
	int read_length;
	unsigned char read_buffer[TCP_READ_BUFFER];
};

extern struct fe_interface_type fe_tcp_type;

#define fe_tcp_connect(server, port)	\
	( (struct fe_tcp *) create_object(OBJECT_TYPE_S(&fe_tcp_type), "nsn", FE_TCP_CONNECT, (server), (port)) )

#define fe_tcp_listen(port)	\
	( (struct fe_tcp *) create_object(OBJECT_TYPE_S(&fe_tcp_type), "nn", FE_TCP_LISTEN, (port)) )

#define fe_tcp_accept(type, inter)	\
	( (struct fe_tcp *) create_object(OBJECT_TYPE_S(type), "np", FE_TCP_ACCEPT, (inter)) )

int fe_tcp_init(struct fe_tcp *, const char *, va_list);
void fe_tcp_release(struct fe_tcp *);

int fe_tcp_read(struct fe_tcp *, char *, int);
int fe_tcp_write(struct fe_tcp *, const char *, int);

int fe_tcp_receive(struct fe_tcp *, char *, int);
int fe_tcp_send(struct fe_tcp *, const char *, int);

int fe_tcp_read_to_buffer(struct fe_tcp *);
int fe_tcp_set_read_buffer(struct fe_tcp *, const char *, int);
int fe_tcp_advance_read_position(struct fe_tcp *, int);
void fe_tcp_clear_buffer(struct fe_tcp *);

#endif

