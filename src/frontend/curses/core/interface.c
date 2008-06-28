/*
 * Module Name:		interface.c
 * Description:		Interface Manager
 */

#include <string.h>
#include <stdarg.h>

#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>

#include <stutter/memory.h>
#include <stutter/object.h>
#include <stutter/globals.h>
#include <stutter/frontend/curses/interface.h>

#define INTERFACE_INIT_SIZE		4
#define INTERFACE_GROWTH_FACTOR		2

struct fe_interface_list {
	int size;
	int next_space;
	struct fe_interface **table;
};

static struct fe_interface_list interface_list;

struct fe_interface_type fe_interface_type = { {
	NULL,
	"interface",
	sizeof(struct fe_interface),
	NULL,
	(object_init_t) fe_interface_init,
	(object_release_t) fe_interface_release },
	(fe_int_read_t) NULL,
	(fe_int_write_t) NULL
};

int init_interface(void)
{
	interface_list.size = INTERFACE_INIT_SIZE;
	interface_list.next_space = 0;
	if (!(interface_list.table = (struct fe_interface **) memory_alloc(interface_list.size * sizeof(struct fe_interface *))))
		return(-1);
	memset(interface_list.table, '\0', interface_list.size * sizeof(struct fe_interface *));
	object_register_type(OBJECT_TYPE_S(&fe_interface_type));
	return(0);
}

void release_interface(void)
{
	int i;

	object_deregister_type(OBJECT_TYPE_S(&fe_interface_type));
	for (i = 0; i < interface_list.size; i++) {
		if (interface_list.table[i])
			destroy_object(OBJECT_S(interface_list.table[i]));
	}
	memory_free(interface_list.table);
}

int fe_interface_init(struct fe_interface *inter, const char *params, va_list va)
{
	int newsize;
	struct fe_interface **newtable;

	inter->read = -1;
	inter->write = -1;
	inter->error = -1;

	if (interface_list.next_space == interface_list.size) {
		newsize = interface_list.size * INTERFACE_GROWTH_FACTOR;
		if (!(newtable = (struct fe_interface **) memory_realloc(interface_list.table, newsize * sizeof(struct fe_interface *))))
			return(-1);
		interface_list.table = newtable;
		memset(&interface_list.table[interface_list.size], '\0', (newsize - interface_list.size) * sizeof(struct fe_interface *));
		interface_list.next_space = interface_list.size;
		interface_list.size = newsize;
	}
	interface_list.table[interface_list.next_space] = inter;
	for (; interface_list.next_space < interface_list.size; interface_list.next_space++) {
		if (!interface_list.table[interface_list.next_space])
			break;
	}
	return(0);
}

void fe_interface_release(struct fe_interface *inter)
{
	int i;

	for (i = 0; i < interface_list.size; i++) {
		if (interface_list.table[i] == inter) {
			interface_list.table[i] = NULL;
			if (i < interface_list.next_space)
				interface_list.next_space = i;
		}
	}
	if (inter->read > 0)
		close(inter->read);
	if ((inter->write > 0) && (inter->write != inter->read))
		close(inter->write);
	if ((inter->error > 0) && (inter->error != inter->read) && (inter->error != inter->write))
		close(inter->error);
}


/**
 * Sets the callback that occurs under the given condition for the given
 * descriptior.
 */
void fe_interface_set_callback(struct fe_interface *inter, int condition, callback_t func, void *ptr)
{
	inter->condition = condition;
	inter->callback.func = func;
	inter->callback.ptr = ptr;
}

/**
 * Returns the callback for the given interface.
 */
struct callback_s fe_interface_get_callback(struct fe_interface *inter)
{
	return(inter->callback);
}


/**
 * Check for activity on all interfaces for up to a maximum of t seconds.
 * If new activity is available, the appropriate callback is called using the
 * interface as the parameter.  The number of interfaces that were serviced is
 * returned or -1 if an error occurred.
 */
int fe_interface_select(float t)
{
	int i;
	int max, ret = 0;
	fd_set rd, wr, err;
	struct timeval timeout;

	/** Check the buffer of each connection to see if any data is waiting
	    and return when each connection gets a chance to read data so that
	    we can check other events and remain responsive */
	for (i = 0; i < interface_list.size; i++) {
		if (!interface_list.table[i])
			continue;
		if ((interface_list.table[i]->condition & IO_COND_READ)
		  && (interface_list.table[i]->bitflags & FE_IO_READY_READ)) {
			EXECUTE_CALLBACK(interface_list.table[i]->callback, interface_list.table[i]);
			ret++;
		}
		if (!interface_list.table[i])
			continue;
		if ((interface_list.table[i]->condition & IO_COND_WRITE)
		  && (interface_list.table[i]->bitflags & FE_IO_READY_WRITE)) {
			EXECUTE_CALLBACK(interface_list.table[i]->callback, interface_list.table[i]);
			ret++;
		}
		if (!interface_list.table[i])
			continue;
		if ((interface_list.table[i]->condition & IO_COND_ERROR)
		  && (interface_list.table[i]->bitflags & FE_IO_READY_ERROR)) {
			EXECUTE_CALLBACK(interface_list.table[i]->callback, interface_list.table[i]);
			ret++;
		}
	}
	if (ret)
		return(ret);

	/** Check each connection's socket for input using select */
	timeout.tv_sec = (int) t;
	timeout.tv_usec = (int) ((t - timeout.tv_sec) * 1000000);

	FD_ZERO(&rd);
	FD_ZERO(&wr);
	FD_ZERO(&err);
	max = 0;
	for (i = 0; i < interface_list.size; i++) {
		if (!interface_list.table[i])
			continue;
		if (interface_list.table[i]->read != -1) {
			FD_SET(interface_list.table[i]->read, &rd);
			if (interface_list.table[i]->read > max)
				max = interface_list.table[i]->read;
		}
		if (interface_list.table[i]->write != -1) {
			FD_SET(interface_list.table[i]->write, &wr);
			if (interface_list.table[i]->write > max)
				max = interface_list.table[i]->write;
		}
		if (interface_list.table[i]->error != -1) {
			FD_SET(interface_list.table[i]->error, &err);
			if (interface_list.table[i]->error > max)
				max = interface_list.table[i]->error;
		}
	}

	if ((ret = select(max + 1, &rd, &wr, &err, &timeout)) == -1) {
		/** There was a socket error so we'll just return */
		return(-1);
	}

	for (i = 0; i < interface_list.size; i++) {
		/** We check that the interface is not NULL before each condition in case the previous
		    callback destroyed the interface */
		if (!interface_list.table[i])
			continue;
		if ((interface_list.table[i]->condition & IO_COND_READ)
		    && ((interface_list.table[i]->read != -1) && FD_ISSET(interface_list.table[i]->read, &rd)))
			EXECUTE_CALLBACK(interface_list.table[i]->callback, interface_list.table[i]);

		if (!interface_list.table[i])
			continue;
		if ((interface_list.table[i]->condition & IO_COND_WRITE)
		    && ((interface_list.table[i]->write != -1) && FD_ISSET(interface_list.table[i]->write, &wr)))
			EXECUTE_CALLBACK(interface_list.table[i]->callback, interface_list.table[i]);

		if (!interface_list.table[i])
			continue;
		if ((interface_list.table[i]->condition & IO_COND_ERROR)
		    && ((interface_list.table[i]->error != -1) && FD_ISSET(interface_list.table[i]->error, &err)))
			EXECUTE_CALLBACK(interface_list.table[i]->callback, interface_list.table[i]);
	}
	return(ret);
}



