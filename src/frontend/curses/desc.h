/*
 * Header Name:		desc.h
 * Version:		0.1
 * Description:		Descriptor Manager Header
 */

#ifndef _CURSES_DESC_H
#define _CURSES_DESC_H

#include <stutter/lib/linear.h>
#include <stutter/lib/globals.h>

#ifndef DESC_READ_BUFFER
#define DESC_READ_BUFFER		512
#endif

struct fe_descriptor_s {
	int read;
	int write;
	int error;
	int read_pos;
	int read_length;
	char read_buffer[DESC_READ_BUFFER];
};

struct fe_descriptor_list_s {
	destroy_t destroy;
	int size;
	int next_space;
	struct fe_descriptor_s **descs;
	linear_node_v(fe_descriptor_list_s) ll;
};

int init_desc(void);
int release_desc(void);

struct fe_descriptor_list_s *fe_desc_create_list(destroy_t);
int fe_desc_destroy_list(struct fe_descriptor_list_s *);
struct fe_descriptor_s *fe_desc_create(struct fe_descriptor_list_s *, int);
int fe_desc_destroy(struct fe_descriptor_list_s *, struct fe_descriptor_s *);
int fe_desc_wait(float);

#endif


