/*
 * Header Name:		net.h
 * Version:		0.1
 * Description:		Frontend Networking Header
 */

#ifndef _STUTTER_FRONTEND_NET_H
#define _STUTTER_FRONTEND_NET_H

#include <stutter/lib/globals.h>

typedef void *fe_network_t;

// TODO what about using non-TCP protocols?  is that important?
fe_network_t fe_net_connect(char *address, int port);
fe_network_t fe_net_listen(int port);
void fe_net_disconnect(fe_network_t net);

int fe_net_send(fe_network_t net, char *buffer, int len);
int fe_net_receive(fe_network_t net, char *buffer, int len);
int fe_net_receive_str(fe_network_t net, char *buffer, int len, char ch);

#endif


