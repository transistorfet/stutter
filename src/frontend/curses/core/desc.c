/*
 * Module Name:		desc.c
 * Version:		0.1
 * System Requirements:	Unix
 * Description:		Descriptor Manager
 */


#include <string.h>

#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>

#include <stutter/signal.h>
#include <stutter/memory.h>
#include <stutter/linear.h>
#include "desc.h"

#define DESC_DEFAULT_DESCRIPTORS		4
#define DESC_GROWTH_FACTOR			2

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
	int i;
	struct fe_descriptor_list_s *cur, *tmp;

	remove_signal("fe.error_ready");
	remove_signal("fe.write_ready");
	remove_signal("fe.read_ready");

	linear_foreach_safe_v(desc_lists, ll, cur, tmp) {
		for (i = 0;i < cur->size;i++) {
			if (cur->descs[i]) {
				if (cur->destroy)
					cur->destroy(cur->descs[i]);
				memory_free(cur->descs[i]);
			}
		}
		memory_free(cur->descs);
		memory_free(cur);
	}
	return(0);
}

/**
 * Close all descriptors without closing the connection.  This function
 * is called by the child process created after a fork.
 */
int fe_desc_close_all(void)
{
	int i;
	struct fe_descriptor_list_s *cur, *tmp;

	linear_foreach_safe_v(desc_lists, ll, cur, tmp) {
		for (i = 0;i < cur->size;i++) {
			if (cur->descs[i]) {
				if (cur->descs[i]->read != -1)
					close(cur->descs[i]->read);
				if (cur->descs[i]->write != -1)
					close(cur->descs[i]->write);
				if (cur->descs[i]->error != -1)
					close(cur->descs[i]->error);
			}
		}
	}
	return(0);
}

/**
 * Create a new list for storing descriptors and return a pointer to it or
 * return NULL on error.  The given destroy function is called when a
 * descriptor in the created list is destroyed.
 */
struct fe_descriptor_list_s *fe_desc_create_list(destroy_t destroy)
{
	struct fe_descriptor_list_s *list;

	if (!(list = (struct fe_descriptor_list_s *) memory_alloc(sizeof(struct fe_descriptor_list_s))))
		return(NULL);
	list->destroy = destroy;
	list->size = DESC_DEFAULT_DESCRIPTORS;
	list->next_space = 0;
	if (!(list->descs = (struct fe_descriptor_s **) memory_alloc(list->size * sizeof(struct fe_descriptor_s *)))) {
		memory_free(list);
		return(NULL);
	}
	memset(list->descs, '\0', list->size * sizeof(struct fe_descriptor_s *));
	linear_add_node_v(desc_lists, ll, list);
	return(list);
}

/**
 * Destroy the given list and all of the descriptors in it.  A 0 is returnred
 * on success or a negative number on error.
 */
int fe_desc_destroy_list(struct fe_descriptor_list_s *list)
{
	int i;

	linear_remove_node_v(desc_lists, ll, list);
	for (i = 0;i < list->size;i++) {
		if (list->descs[i]) {
			if (list->destroy)
				list->destroy(list->descs[i]);
			memory_free(list->descs[i]);
		}
	}
	memory_free(list->descs);
	memory_free(list);
	return(0);
}

/**
 * Create a new descriptor in the given list with the given size allocated for
 * it.  If the size is 0 then the size of the fe_descriptor_s struct is used.
 * A pointer to the descriptor is returned on NULL on error.
 */
struct fe_descriptor_s *fe_desc_create(struct fe_descriptor_list_s *list, int size)
{
	int newsize;
	struct fe_descriptor_s *desc;

	if (!size)
		size = sizeof(struct fe_descriptor_s);
	if (!(desc = (struct fe_descriptor_s *) memory_alloc(size)))
		return(NULL);
	memset(desc, '\0', size);

	if (list->next_space == list->size) {
		newsize = list->size * DESC_GROWTH_FACTOR;
		if (!(list->descs = (struct fe_descriptor_s **) memory_realloc(list->descs, newsize * sizeof(struct fe_descriptor_s *)))) {
			memory_free(desc);
			return(NULL);
		}
		memset(&list->descs[list->size], '\0', (newsize - list->size) * sizeof(struct fe_descriptor_s *));
		list->next_space = list->size;
		list->size = newsize;
	}
	list->descs[list->next_space] = desc;
	for (;list->next_space < list->size;list->next_space++) {
		if (!list->descs[list->next_space])
			break;
	}
	return(desc);
}

/**
 * Destroy the given descriptor in the given list.  The destroy function stored
 * in the list is called before the memory for the descriptor is freed.  A 0 is
 * returned on success or a negative number on error.
 */
int fe_desc_destroy(struct fe_descriptor_list_s *list, struct fe_descriptor_s *desc)
{
	int i;

	if (list->destroy)
		list->destroy(desc);
	memory_free(desc);

	for (i = 0;i < list->size;i++) {
		if (list->descs[i] == desc) {
			list->descs[i] = NULL;
			if (i < list->next_space)
				list->next_space = i;
		}
	}	
	return(0);
}


/**
 * Send the string of length len to the given process and
 * return the number of bytes written or -1 on error.
 */
int fe_desc_write(struct fe_descriptor_s *desc, char *buffer, int len)
{
	int sent, count = 0;

	if (!desc)
		return(0);
	do {
		if ((sent = write(desc->read, (void *) buffer, len)) < 0)
			return(-1);
		else if (!sent)
			return(0);
		count += sent;
	} while (count < len);
	return(count);
}

/**
 * Receive the given number of bytes, store them in the given buffer
 * and return the number of bytes read or -1 on error.
 */ 
int fe_desc_read(struct fe_descriptor_s *desc, char *buffer, int len)
{
	int i, j;
	fd_set rd;
	struct timeval timeout = { 0, 0 };

	if (!desc)
		return(-1);

	len--;
	for (i = 0;i < len;i++) {
		if (desc->read_pos >= desc->read_length)
			break;
		buffer[i] = desc->read_buffer[desc->read_pos++];
	}

	if (i < len) {
		FD_ZERO(&rd);
		FD_SET(desc->read, &rd);
		if (select(desc->read + 1, &rd, NULL, NULL, &timeout) && ((j = read(desc->read, &buffer[i], len - i)) > 0))
			i += j;
		if (j <= 0)
			return(-1);
	}

	buffer[i] = '\0';
	return(i);
}

/**
 * Receive a string from the descriptor up to a maximum of
 * len-1 (a null char is appended) and return the number of bytes
 * read or -1 on error or disconnect.
 */ 
int fe_desc_read_str(struct fe_descriptor_s *desc, char *buffer, int len, char ch)
{
	int i;
	fd_set rd;
	struct timeval timeout = { 0, 0 };

	if (!desc)
		return(-1);

	len--;
	for (i = 0;i < len;i++) {
		if (desc->read_pos >= desc->read_length) {
			FD_ZERO(&rd);
			FD_SET(desc->read, &rd);
			// TODO if either of these fail, we return a partial message which the server
			//	doesn't check for and can't deal with so what we really need to do is
			//	copy what we've read (in buffer) to the buffer and return 0 but what
			//	if the buffer is bigger than the buffer?
			// TODO what about a socket error of some sorts too.  That doesn't really count
			//	as a buffer that hasn't been fully recieved.  We could send a signal to report
			//	the error somehow (which raises the question of should we do (and how would
			//	we do) socket specific signals.
			if (!select(desc->read + 1, &rd, NULL, NULL, &timeout)) {
				buffer[i + 1] = '\0';
				return(i);
			}
			if ((desc->read_length = read(desc->read, desc->read_buffer, DESC_READ_BUFFER)) <= 0)
				return(-1);
			desc->read_pos = 0;
		}
		buffer[i] = desc->read_buffer[desc->read_pos++];
		if (buffer[i] == ch)
			break;
	}

	buffer[i + 1] = '\0';
	return(i);
}


/**
 * Check for activity on all descriptors in all lists up to a maximum of
 * t seconds.  If new activity is available, the appropriate signal,
 * fe.read_ready, fe.write_ready, or fe.error_ready is emitted using the
 * descriptor as the index and the parameter.  The number of descriptors
 * that were ready is returned.
 */
int fe_desc_wait(float t)
{
	int i;
	int max, ret = 0;
	fd_set rd, wr, err;
	struct timeval timeout;
	struct fe_descriptor_list_s *cur;

	/** Check the buffer of each connection to see if any messages are waiting
	    and return when each connection gets a chance to read one message so that
	    we can refresh the screen and check for keyboard input to remain responsive */
	linear_foreach_v(desc_lists, ll, cur) {
		for (i = 0;i < cur->size;i++) {
			if (cur->descs[i] && (cur->descs[i]->read_pos < cur->descs[i]->read_length)) {
				emit_signal("fe.read_ready", cur->descs[i], cur->descs[i]);
				ret++;
			}
		}
	}
	if (ret)
		return(ret);

	/** Check each connection's socket for input using select */
	timeout.tv_sec = (int) t;
	timeout.tv_usec = (int) ((t - timeout.tv_sec) * 1000000);

	FD_ZERO(&rd);
	FD_SET(0, &rd);
	FD_ZERO(&wr);
	FD_ZERO(&err);
	max = 0;
	linear_foreach_v(desc_lists, ll, cur) {
		for (i = 0;i < cur->size;i++) {
			if (!cur->descs[i])
				continue;
			if (cur->descs[i]->read != -1) {
				FD_SET(cur->descs[i]->read, &rd);
				if (cur->descs[i]->read > max)
					max = cur->descs[i]->read;
			}
			if (!cur->descs[i])
				continue;
			if (cur->descs[i]->write != -1) {
				FD_SET(cur->descs[i]->write, &wr);
				if (cur->descs[i]->write > max)
					max = cur->descs[i]->write;
			}
			if (!cur->descs[i])
				continue;
			if (cur->descs[i]->error != -1) {
				FD_SET(cur->descs[i]->error, &err);
				if (cur->descs[i]->error > max)
					max = cur->descs[i]->error;
			}
		}
	}

	if ((ret = select(max + 1, &rd, &wr, &err, &timeout)) == -1) {
		// TODO what do we do in the case of a socket error?
		return(-1);
	}

	linear_foreach_v(desc_lists, ll, cur) {
		for (i = 0;i < cur->size;i++) {
			if (!cur->descs[i])
				continue;
			if ((cur->descs[i]->read_pos < cur->descs[i]->read_length) || ((cur->descs[i]->read != -1) && FD_ISSET(cur->descs[i]->read, &rd)))
				emit_signal("fe.read_ready", cur->descs[i], cur->descs[i]);
			if (!cur->descs[i])
				continue;
			if ((cur->descs[i]->write != -1) && FD_ISSET(cur->descs[i]->write, &wr))
				emit_signal("fe.write_ready", cur->descs[i], cur->descs[i]);
			if (!cur->descs[i])
				continue;
			if ((cur->descs[i]->error != -1) && FD_ISSET(cur->descs[i]->error, &err))
				emit_signal("fe.error_ready", cur->descs[i], cur->descs[i]);
		}
	}
	return(ret);
}


