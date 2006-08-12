/*
 * Header Name:		net.h
 * Version:		0.1
 * Description:		Network Interface Manager Header
 */

#ifndef _WIN32_NET_H
#define _WIN32_NET_H

#include <stutter/lib/globals.h>

#define NET_ATTEMPTS		3

#define NET_MESSAGE		1000

struct network_s;
typedef struct network_s *network_t;

network_t fe_net_connect(char *, int, callback_t, void *);
network_t fe_net_listen(int, callback_t, void *);
int fe_net_set_receiver(network_t, callback_t, void *);
void fe_net_disconnect(network_t);
int fe_net_send(network_t, char *, int);
int fe_net_receive(network_t, char *, int);
int fe_net_handle_message(int, int, int);

#endif

