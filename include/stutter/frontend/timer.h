/*
 * Header Name:		timer.h
 * Description:		Frontend Timer Header
 */

#ifndef _STUTTER_FRONTEND_TIMER_H
#define _STUTTER_FRONTEND_TIMER_H

#include <stutter/globals.h>

#define FE_TIMER_BF_ONE_TIME		0x00
#define FE_TIMER_BF_PERIODIC		0x01
#define FE_TIMER_BF_EXPIRED		0x80

struct fe_timer;
typedef struct fe_timer *fe_timer_t;

fe_timer_t fe_timer_create(int bitflags, float interval, callback_t func, void *ptr);
void fe_timer_destroy(fe_timer_t timer);

struct callback_s fe_timer_get_callback(fe_timer_t timer);
void fe_timer_set_callback(fe_timer_t timer, callback_t func, void *ptr);

int fe_timer_reset(fe_timer_t timer);
int fe_timer_expire(fe_timer_t timer);
int fe_timer_set_interval(fe_timer_t timer, float interval);

#endif

