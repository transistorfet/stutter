/*
 * Header Name:		net.h
 * Version:		0.1
 * Description:		Frontend Networking Header
 */

#ifndef _STUTTER_FRONTEND_NET_H
#define _STUTTER_FRONTEND_NET_H

#include <stutter/lib/globals.h>

typedef void *network_t;

// TODO what about using non-TCP protocols?  is that important?
network_t fe_net_connect(char *address, int port, callback_t receiver, void *ptr);
network_t fe_net_listen(int port, callback_t receiver, void *ptr);
int fe_net_set_receiver(network_t net, callback_t receiver, void *ptr);
void fe_net_disconnect(network_t net);
int fe_net_send(network_t net, char *buffer, int len);
int fe_net_receive(network_t net, char *buffer, int len);
int fe_net_receive_str(network_t net, char *buffer, int len, char ch);

#endif


