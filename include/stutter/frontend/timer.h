/*
 * Header Name:		timer.h
 * Version:		0.1
 * Description:		Frontend Timer Header
 */

#ifndef _STUTTER_FRONTEND_TIMER_H
#define _STUTTER_FRONTEND_TIMER_H

#define FE_TIMER_BF_ONE_TIME		0x00
#define FE_TIMER_BF_PERIODIC		0x01
#define FE_TIMER_BF_EXPIRED		0x80

typedef void *fe_timer_t;

fe_timer_t fe_timer_create(int bitflags, float interval);
void fe_timer_destroy(fe_timer_t timer);

int fe_timer_reset(fe_timer_t timer);
int fe_timer_set_interval(fe_timer_t timer, float interval);


#endif

