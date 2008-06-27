/*
 * Module Name:		queue.h
 * Description:		Queue
 */

#include <stutter/queue.h>
#include <stutter/memory.h>
#include <stutter/globals.h>

struct queue_s *create_queue(int max, destroy_t destroy)
{
	struct queue_s *queue;

	if (!(queue = (struct queue_s *) memory_alloc(sizeof(struct queue_s))))
		return(NULL);
	queue->max = max;
	queue->size = 0;
	queue->destroy = destroy;
	queue->current = NULL;
	queue->head = NULL;
	queue->tail = NULL;
	return(queue);
}

void destroy_queue(struct queue_s *queue)
{
	queue_clear(queue);
	memory_free(queue);
}

int queue_prepend(struct queue_s *queue, void *data)
{
	struct queue_entry_s *entry;

	if (!(entry = memory_alloc(sizeof(struct queue_entry_s))))
		return(-1);
	entry->data = data;
	entry->next = queue->head;
	entry->prev = NULL;
	if (queue->head)
		queue->head->prev = entry;
	queue->head = entry;
	if (!queue->tail)
		queue->tail = entry;
	queue->size++;
	if (queue->max && (queue->size > queue->max)) {
		if (!(data = queue_pop(queue)))
			return(1);
		queue->destroy(data);
	}
	return(0);
}

int queue_append(struct queue_s *queue, void *data)
{
	struct queue_entry_s *entry;

	if (queue->max && (queue->size >= queue->max))
		return(1);
	if (!(entry = memory_alloc(sizeof(struct queue_entry_s))))
		return(-1);
	entry->data = data;
	entry->next = NULL;
	entry->prev = queue->tail;
	if (queue->tail)
		queue->tail->next = entry;
	else
		queue->head = entry;
	queue->tail = entry;
	queue->size++;
	return(0);
}

int queue_insert(struct queue_s *queue, void *data)
{
	void *tmp;
	struct queue_entry_s *entry;

	// TODO re-evaluate this function's semantics based on the common behaviour
	//	when attempting to insert an element in an ordered queue
	if (!queue->current)
		return(1);
	if (queue->max && (queue->size > queue->max)) {
		if (!(tmp = queue_pop(queue)))
			return(1);
		queue->destroy(tmp);
	}
	if (!(entry = memory_alloc(sizeof(struct queue_entry_s))))
		return(-1);
	entry->data = data;
	entry->prev = queue->current;
	entry->next = queue->current->next;
	if (queue->current->next)
		queue->current->next->prev = entry;
	queue->current->next = entry;
	if (queue->tail == queue->current)
		queue->tail = entry;
	queue->size++;
	return(0);
}

void *queue_shift(struct queue_s *queue)
{
	void *data;
	struct queue_entry_s *entry;

	if (!queue->head)
		return(NULL);
	entry = queue->head;
	data = entry->data;
	queue->head = queue->head->next;
	if (queue->head)
		queue->head->prev = NULL;
	else
		queue->tail = NULL;
	if (queue->current == entry)
		queue->current = NULL;
	queue->size--;
	memory_free(entry);
	return(data);
}

void *queue_pop(struct queue_s *queue)
{
	void *data;
	struct queue_entry_s *entry;

	if (queue->tail)
		return(NULL);
	entry = queue->tail;
	data = entry->data;
	queue->tail = queue->tail->prev;
	if (queue->tail)
		queue->tail->next = NULL;
	else
		queue->head = NULL;
	queue->size--;
	if (queue->current == entry)
		queue->current = NULL;
	return(data);
}

void *queue_remove(struct queue_s *queue)
{
	void *data;

	if (!queue->current)
		return(NULL);
	if (queue->current->prev)
		queue->current->prev->next = queue->current->next;
	else
		queue->head = queue->current->next;
	if (queue->current->next)
		queue->current->next->prev = queue->current->prev;
	else
		queue->tail = queue->current->prev;
	data = queue->current->data;
	memory_free(queue->current);
	queue->current = queue->current->next;
	return(data);
}

void queue_delete(struct queue_s *queue)
{
	if (!queue->current)
		return;
	if (queue->current->prev)
		queue->current->prev->next = queue->current->next;
	else
		queue->head = queue->current->next;
	if (queue->current->next)
		queue->current->next->prev = queue->current->prev;
	else
		queue->tail = queue->current->prev;
	queue->destroy(queue->current->data);
	memory_free(queue->current);
	queue->current = queue->current->next;
}

void queue_clear(struct queue_s *queue)
{
	struct queue_entry_s *cur, *tmp;

	for (cur = queue->head; cur; cur = tmp) {
		tmp = cur->next;
		queue->destroy(cur->data);
		memory_free(cur);
	}
	queue->head = NULL;
	queue->tail = NULL;
	queue->current = NULL;
}


