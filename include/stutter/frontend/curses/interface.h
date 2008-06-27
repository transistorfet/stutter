/*
 * Header Name:		interface.h
 * Description:		Interface Header
 */

#ifndef _STUTTER_FRONTEND_CURSES_INTERFACE_H
#define _STUTTER_FRONTEND_CURSES_INTERFACE_H

#include <stdarg.h>

#include <stutter/object.h>
#include <stutter/globals.h>

#define FE_IO_READY_READ		0x0010
#define FE_IO_READY_WRITE		0x0020
#define FE_IO_READY_ERROR		0x0040

#define FE_IBF_RELEASING		0x1000

#define FE_INTERFACE(ptr)		( (struct fe_interface *) (ptr) )
#define FE_INTERFACE_TYPE(ptr)		( (struct fe_interface_type *) (ptr) )
#define FE_INTERFACE_GET_TYPE(ptr)	( FE_INTERFACE_TYPE( OBJECT_S(ptr)->type ) )

struct fe_interface;
struct fe_interface_type;

typedef int (*fe_int_read_t)(struct fe_interface *, char *, int);
typedef int (*fe_int_write_t)(struct fe_interface *, const char *, int);

#define FE_INTERFACE_READ(inter, buffer, max)	\
	( FE_INTERFACE_GET_TYPE(inter)->read(FE_INTERFACE(inter), (buffer), (max)) )
#define FE_INTERFACE_WRITE(inter, str, len)	\
	( FE_INTERFACE_GET_TYPE(inter)->write(FE_INTERFACE(inter), (str), (len)) )

#define FE_INTERFACE_SET_READY_READ(inter)	( FE_INTERFACE(inter)->bitflags |= FE_IO_READY_READ )
#define FE_INTERFACE_SET_NOT_READY_READ(inter)	( FE_INTERFACE(inter)->bitflags &= ~FE_IO_READY_READ )

struct fe_interface {
	struct object_s object;
	int bitflags;
	int condition;
	struct callback_s callback;
	int read;
	int write;
	int error;
};

struct fe_interface_type {
	struct object_type_s object_type;
	fe_int_read_t read;
	fe_int_write_t write;
};

extern struct fe_interface_type fe_interface_type;

int init_interface(void);
void release_interface(void);

int fe_interface_init(struct fe_interface *, const char *, va_list);
void fe_interface_release(struct fe_interface *);

void fe_interface_set_callback(struct fe_interface *, int, callback_t, void *);
struct callback_s fe_interface_get_callback(struct fe_interface *);

int fe_interface_select(float);

#endif

