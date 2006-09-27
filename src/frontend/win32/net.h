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

struct fe_network_s;
typedef struct fe_network_s *fe_network_t;

fe_network_t fe_net_connect(char *, int);
fe_network_t fe_net_listen(int);
void fe_net_disconnect(fe_network_t);
int fe_net_send(fe_network_t, char *, int);
int fe_net_receive(fe_network_t, char *, int);
int fe_net_handle_message(int, int, int);

#endif


