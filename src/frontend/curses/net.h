/*
 * Header Name:		net.h
 * Version:		0.1
 * Description:		Network Interface Manager Header
 */

#ifndef _SIMPLE_NET_H
#define _SIMPLE_NET_H

#include <stutter/lib/globals.h>

#define NET_ATTEMPTS		3

struct fe_network_s;
typedef struct fe_network_s *fe_network_t;

int init_net(void);
int release_net(void);

fe_network_t fe_net_connect(char *, int, callback_t, void *);
fe_network_t fe_net_listen(int, callback_t, void *);
int fe_net_set_receiver(fe_network_t, callback_t, void *);
void fe_net_disconnect(fe_network_t);
int fe_net_send(fe_network_t, char *, int);
int fe_net_receive(fe_network_t, char *, int);
int fe_net_wait(float);

#endif


