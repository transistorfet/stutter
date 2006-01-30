/*
 * Header Name:		net.h
 * Version:		0.1
 * Description:		Network Interface Manager Header
 */

#ifndef _COMMON_NET_H
#define _COMMON_NET_H

#include <lib/globals.h>

#define NET_ATTEMPTS		3

#ifdef NET_DIRECT
#define net_connect		fe_net_connect
#define net_listen		fe_net_listen
#define net_set_receiver	fe_net_set_receiver
#define net_disconnect		fe_net_disconnect
#define net_send		fe_net_send
#define net_receive		fe_net_receive
#define net_wait		fe_net_wait
#endif

struct network_s;
typedef struct network_s *network_t;

network_t net_connect(char *, int, callback_t, void *);
network_t net_listen(int, callback_t, void *);
int net_set_receiver(network_t, callback_t, void *);
void net_disconnect(network_t);
int net_send(network_t, char *, int);
int net_receive(network_t, char *, int);
int net_wait(float);

#endif


