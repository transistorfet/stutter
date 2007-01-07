/*
 * Module Name:		timer.c
 * Version:		0.1
 * System Requirements:	(none)
 * Description:		Timer Manager
 */


#include <time.h>

#include <stutter/signal.h>
#include <stutter/memory.h>
#include <stutter/frontend/timer.h>

struct fe_timer_s {
	int bitflags;
	float interval;
	time_t start;
	struct fe_timer_s *prev;
	struct fe_timer_s *next;
};

struct fe_timer_s *timer_list = NULL;

static void fe_timer_insert(struct fe_timer_s *);
static void fe_timer_remove(struct fe_timer_s *);

int init_timer(void)
{
	add_signal("fe.timer_done", 0);
	return(0);
}

int release_timer(void)
{
	struct fe_timer_s *cur, *tmp;

	for (cur = timer_list;cur;) {
		tmp = cur->next;
		memory_free(cur);
		cur = tmp;
	}
	remove_signal("fe.timer_done");
	return(0);
}

/**
 * Execute the given command and return a reference to the running program.
 */
fe_timer_t fe_timer_create(int bitflags, float interval)
{
	struct fe_timer_s *timer, *cur, *prev;

	if (!(timer = (struct fe_timer_s *) memory_alloc(sizeof(struct fe_timer_s))))
		return(NULL);
	timer->bitflags = bitflags;
	timer->interval = interval;
	timer->start = time(NULL);
	timer->prev = NULL;
	timer->next = NULL;

	fe_timer_insert(timer);
	return((fe_timer_t) timer);
}

/**
 * Close the given program reference.
 */
void fe_timer_destroy(fe_timer_t timer)
{
	fe_timer_remove((struct fe_timer_s *) timer);
	memory_free(timer);
}

/**
 * Reset the given timer's start time to the current time.  If the timer has
 * already expired, it will start counting again.  If an error occurs, -1 is
 * returned; otherwise 0 is returned.
 */
int fe_timer_reset(fe_timer_t timer)
{
	fe_timer_remove((struct fe_timer_s *) timer);
	((struct fe_timer_s *) timer)->bitflags &= ~FE_TIMER_BF_EXPIRED;
	((struct fe_timer_s *) timer)->start = time(NULL);
	fe_timer_insert((struct fe_timer_s *) timer);
	return(0);
}

/**
 * Set the given timer's interval to the given interval without resetting
 * the timer's time so far.  If the timer has already expired, 1 will be
 * returned.  If an error occurs, -1 will be returned.  Otherwise 0 is
 * returned.
 */
int fe_timer_set_interval(fe_timer_t timer, float interval)
{
	fe_timer_remove((struct fe_timer_s *) timer);
	((struct fe_timer_s *) timer)->interval = interval;
	fe_timer_insert((struct fe_timer_s *) timer);
	return(0);
}

/**
 * Check all timers for expiration.
 */
int fe_timer_check(void)
{
	time_t current_time;
	struct fe_timer_s *cur;

	current_time = time(NULL);
	for (cur = timer_list;cur;cur = cur->next) {
		if (cur->bitflags & FE_TIMER_BF_EXPIRED)
			continue;
		else if ((current_time - cur->start) >= cur->interval) {
			cur->bitflags |= FE_TIMER_BF_EXPIRED;
			emit_signal("fe.timer_done", cur, cur);
			if (cur->bitflags & FE_TIMER_BF_PERIODIC)
				fe_timer_reset((fe_timer_t) cur);
		}
		else
			break;
	}
	return(0);
}

/*** Local Functions ***/

static void fe_timer_insert(struct fe_timer_s *timer)
{
	double expiration;
	struct fe_timer_s *cur, *prev;

	if (!timer_list) {
		timer_list = timer;
		timer->next = NULL;
		timer->prev = NULL;
	}
	else {
		expiration = timer->start + timer->interval;
		for (prev = NULL, cur = timer_list;cur;prev = cur, cur = cur->next) {
			if (expiration <= (cur->start + cur->interval))
				break;
		}
		timer->prev = prev;
		timer->next = cur;
		if (prev)
			prev->next = timer;
		else
			timer_list = timer;
		if (cur)
			cur->prev = timer;
	}
}

static void fe_timer_remove(struct fe_timer_s *timer)
{
	if (timer->prev)
		timer->prev->next = timer->next;
	else
		timer_list = timer->next;
	if (timer->next)
		timer->next->prev = timer->prev;
}

