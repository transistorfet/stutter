/*
 * Header Name:		net.h
 * Description:		Frontend Networking Header
 */

#ifndef _STUTTER_FRONTEND_NET_H
#define _STUTTER_FRONTEND_NET_H

#include <stutter/globals.h>

typedef void *fe_network_t;

// TODO what about using non-TCP protocols?  is that important?
fe_network_t fe_net_connect(char *address, int port);
fe_network_t fe_net_listen(int port);
void fe_net_disconnect(fe_network_t net);

struct callback_s fe_net_get_callback(fe_network_t net);
void fe_net_set_callback(fe_network_t net, int condition, callback_t func, void *ptr);

int fe_net_send(fe_network_t net, char *buffer, int len);
int fe_net_receive(fe_network_t net, char *buffer, int len);
int fe_net_receive_str(fe_network_t net, char *buffer, int len, char ch);

#endif


