/*
 * Module Name:		timer.c
 * Description:		Timer Manager
 */


#include <time.h>
#include <windows.h>

#include <stutter/signal.h>
#include <stutter/memory.h>
#include <stutter/globals.h>
#include <stutter/frontend/timer.h>

struct fe_timer_s {
	int bitflags;
	struct callback_s callback;
	float interval;
	time_t start;
	struct fe_timer_s *prev;
	struct fe_timer_s *next;
};

struct fe_timer_s *timer_list = NULL;

static void fe_timer_insert(struct fe_timer_s *);
static void fe_timer_remove(struct fe_timer_s *);
static void CALLBACK fe_timer_callback(HWND, UINT, UINT, DWORD);

int init_timer(void)
{
	return(0);
}

int release_timer(void)
{
	struct fe_timer_s *cur, *tmp;

	KillTimer(NULL, 0);
	for (cur = timer_list;cur;) {
		tmp = cur->next;
		memory_free(cur);
		cur = tmp;
	}
	return(0);
}

/**
 * Execute the given command and return a reference to the running program.
 */
fe_timer_t fe_timer_create(int bitflags, float interval, callback_t func, void *ptr)
{
	struct fe_timer_s *timer;

	if (!(timer = (struct fe_timer_s *) memory_alloc(sizeof(struct fe_timer_s))))
		return(NULL);
	timer->bitflags = bitflags;
	timer->callback.func = func;
	timer->callback.ptr = ptr;
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
 * Returns the callback for the given timer.
 */
struct callback_s fe_timer_get_callback(fe_timer_t timer)
{
	return(((struct fe_timer_s *) timer)->callback);
}

/**
 * Sets the callback of the given timer.
 */
void fe_timer_set_callback(fe_timer_t timer, callback_t func, void *ptr)
{
	((struct fe_timer_s *) timer)->callback.func = func;
	((struct fe_timer_s *) timer)->callback.ptr = ptr;
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
 * Expires the given timer.  If an error occurs then -1 is returned,
 * otherwise 0 is returned.
 */
int fe_timer_expire(fe_timer_t timer)
{
	fe_timer_remove((struct fe_timer_s *) timer);
	((struct fe_timer_s *) timer)->bitflags |= FE_TIMER_BF_EXPIRED;
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

/*** Local Functions ***/

static void fe_timer_insert(struct fe_timer_s *timer)
{
	double expiration;
	struct fe_timer_s *cur, *prev;

	if (!timer_list) {
		timer_list = timer;
		timer->next = NULL;
		timer->prev = NULL;
		SetTimer(NULL, 0, (UINT) (timer->interval * 1000), fe_timer_callback);
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
		if (!timer->prev || (timer->prev->bitflags & FE_TIMER_BF_EXPIRED))
			SetTimer(NULL, 0, (UINT) (timer->interval - (time(NULL) - timer->start)) * 1000, fe_timer_callback);
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

static void CALLBACK fe_timer_callback(HWND hwnd, UINT message, UINT idTimer, DWORD dwTime)
{
	time_t current_time;
	struct fe_timer_s *cur;

	current_time = time(NULL);
	for (cur = timer_list;cur;cur = cur->next) {
		if (cur->bitflags & FE_TIMER_BF_EXPIRED)
			continue;
		if ((current_time - cur->start) >= cur->interval) {
			cur->bitflags |= FE_TIMER_BF_EXPIRED;
			execute_callback_m(cur->callback, cur);
			if (cur->bitflags & FE_TIMER_BF_PERIODIC)
				fe_timer_reset((fe_timer_t) cur);
		}
		else {
			SetTimer(NULL, 0, (UINT) (cur->interval - (current_time - cur->start)) * 1000, fe_timer_callback);
			break;
		}
	}
	if (!cur)
		KillTimer(NULL, 0);
}


