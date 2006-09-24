/*
 * Module Name:		desc.c
 * Version:		0.1
 * System Requirements:	Unix
 * Description:		Descriptor Manager
 */


#include <string.h>

#include <sys/types.h>
#include <sys/select.h>

#include <stutter/lib/memory.h>
#include <stutter/lib/linear.h>
#include "desc.h"

linear_list_v(fe_descriptor_list_s) desc_lists;

int init_desc(void)
{
	linear_init_v(desc_lists);
	add_signal("fe.read_ready", 0);
	add_signal("fe.write_ready", 0);
	add_signal("fe.error_ready", 0);
	return(0);
}

int release_desc(void)
{
	struct fe_descriptor_list_s *list;

	remove_signal("fe.error_ready");
	remove_signal("fe.write_ready");
	remove_signal("fe.read_ready");

	linear_destroy_list_v(desc_lists, ll,
		list = cur;
		linear_destroy_list_v(list->dl, dl,
			if (list->destroy)
				list->destroy(cur);
			memory_free(cur);
		);
		memory_free(list);
	);
	return(0);
}

struct fe_descriptor_list_s *fe_desc_create_list(destroy_t destroy)
{
	struct fe_descriptor_list_s *list;

	if (!(list = (struct fe_descriptor_list_s *) memory_alloc(sizeof(struct fe_descriptor_list_s))))
		return(NULL);
	list->destroy = destroy;
	linear_init_v(list->dl);
	linear_add_node_v(desc_lists, ll, list);
	return(list);
}

int fe_desc_destroy_list(struct fe_descriptor_list_s *list)
{
	linear_destroy_list_v(list->dl, dl,
		if (list->destroy)
			list->destroy(cur);
		memory_free(cur);
	);
	linear_remove_node_v(desc_lists, ll, list);
	memory_free(list);
	return(0);
}

struct fe_descriptor_s *fe_desc_create(struct fe_descriptor_list_s *list, int size)
{
	struct fe_descriptor_s *desc;

	if (!size)
		size = sizeof(struct fe_descriptor_s);
	if (!(desc = (struct fe_descriptor_s *) memory_alloc(size)))
		return(NULL);
	memset(desc, '\0', size);
	linear_add_node_v(list->dl, dl, desc);
	return(desc);
}

int fe_desc_destroy(struct fe_descriptor_list_s *list, struct fe_descriptor_s *desc)
{
	if (list->destroy)
		list->destroy(desc);
	linear_remove_node_v(list->dl, dl, desc);
	memory_free(desc);
	return(0);
}

/**
 *
 */
int fe_desc_wait(float t)
{
	fd_set rd;
	int max, ret = 0;
	struct timeval timeout;
	struct fe_descriptor_list_s *list;

	/** Check the buffer of each connection to see if any messages are waiting
	    and return when each connection gets a chance to read one message so that
	    we can refresh the screen and check for keyboard input to remain responsive */
	linear_traverse_list_v(desc_lists, ll,
		list = cur;
		linear_traverse_list_v(list->dl, dl,
			if (cur->read_pos < cur->read_length) {
				emit_signal("fe.read_ready", cur, cur);
				ret++;
			}
		);
	);
	if (ret)
		return(ret);

	/** Check each connection's socket for input using select */
	timeout.tv_sec = (int) t;
	timeout.tv_usec = (int) ((t - timeout.tv_sec) * 1000000);

	FD_ZERO(&rd);
	FD_SET(0, &rd);
	max = 0;
	linear_traverse_list_v(desc_lists, ll,
		list = cur;
		linear_traverse_list_v(list->dl, dl,
			if (cur->read != -1) {
				FD_SET(cur->read, &rd);
				if (cur->read > max)
					max = cur->read;
			}
		);
	);

	if ((ret = select(max + 1, &rd, NULL, NULL, &timeout)) == -1) {
		// TODO what do we do in the case of a socket error?
		return(-1);
	}

	linear_traverse_list_v(desc_lists, ll,
		list = cur;
		linear_traverse_list_v(list->dl, dl,
			if ((cur->read_pos < cur->read_length) || FD_ISSET(cur->read, &rd))
				emit_signal("fe.read_ready", cur, cur);
		);
	);
	return(ret);
}


