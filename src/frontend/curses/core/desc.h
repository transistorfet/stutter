/*
 * Header Name:		desc.h
 * Description:		Descriptor Manager Header
 */

#ifndef _CURSES_DESC_H
#define _CURSES_DESC_H

#include <stutter/linear.h>
#include <stutter/globals.h>

#ifndef DESC_READ_BUFFER
#define DESC_READ_BUFFER		512
#endif

struct fe_descriptor_s {
	int bitflags;
	int condition;
	struct callback_s callback;
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
int fe_desc_close_all(void);

struct fe_descriptor_list_s *fe_desc_create_list(destroy_t);
int fe_desc_destroy_list(struct fe_descriptor_list_s *);
struct fe_descriptor_s *fe_desc_create(struct fe_descriptor_list_s *, int);
int fe_desc_destroy(struct fe_descriptor_list_s *, struct fe_descriptor_s *);

struct callback_s fe_desc_get_callback(struct fe_descriptor_s *);
void fe_desc_set_callback(struct fe_descriptor_s *, int, callback_t, void *);

int fe_desc_write(struct fe_descriptor_s *, char *, int);
int fe_desc_read(struct fe_descriptor_s *, char *, int);
int fe_desc_read_str(struct fe_descriptor_s *, char *, int, char);

int fe_desc_wait(float);

#endif


