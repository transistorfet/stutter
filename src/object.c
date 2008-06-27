/*
 * Module Name:		object.c
 * Description:		Base Object
 */

#include <stdarg.h>
#include <string.h>

#include <stutter/hash.h>
#include <stutter/memory.h>
#include <stutter/object.h>

static struct hash_s *object_type_list = NULL;

int init_object(void)
{
	if (object_type_list)
		return(1);
	if (!(object_type_list = create_hash(0, -1, NULL)))
		return(-1);
	return(0);
}

void release_object(void)
{
	if (!object_type_list)
		return;
	destroy_hash(object_type_list);
	object_type_list = NULL;
}

int object_register_type(struct object_type_s *type)
{
	return(hash_add(object_type_list, type->name, -1, type));
}

int object_deregister_type(struct object_type_s *type)
{
	return(hash_remove(object_type_list, type->name, -1));
}

struct object_type_s *object_find_type(const char *name, struct object_type_s *base)
{
	struct object_type_s *type, *cur;

	if (!(type = hash_find(object_type_list, name, -1)))
		return(NULL);
	/** If base is given, then only return this type if it is a subclass of base */
	if (!base)
		return(type);
	for (cur = type; cur; cur = cur->parent) {
		if (cur == base)
			return(type);
	}
	return(NULL);
}


struct object_s *create_object(struct object_type_s *type, const char *params, ...)
{
	va_list va;

	va_start(va, params);
	return(create_object_real(type, params, va));
}

struct object_s *create_object_real(struct object_type_s *type, const char *params, va_list va)
{
	struct object_s *obj;

	if (!type || !(obj = memory_alloc(type->size)))
		return(NULL);
	memset(obj, '\0', type->size);
	obj->type = type;

	if (type->init && type->init(obj, params, va)) {
		destroy_object(obj);
		return(NULL);
	}
	va_end(va);
	return(obj);
}

void destroy_object(struct object_s *obj)
{
	/** We don't want to attempt to free this object twice */
	if (!obj || (obj->bitflags & OBJECT_BF_RELEASING))
		return;
	obj->bitflags |= OBJECT_BF_RELEASING;
	if (obj->type->release)
		obj->type->release(obj);
	memory_free(obj);
}



