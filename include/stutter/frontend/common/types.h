/*
 * Header Name:		types.h
 * Description:		Common Frontend Types Header
 */

#ifndef _STUTTER_FRONTEND_COMMON_TYPES_H
#define _STUTTER_FRONTEND_COMMON_TYPES_H

#include <stdlib.h>

#include <stutter/object.h>
#include <stutter/globals.h>
#include <stutter/variable.h>
#include <stutter/frontend/common/surface.h>

/*** Attrib Type ***/

#define FE_ATTRIB(ptr)		( (struct fe_attrib *) (ptr) )

struct fe_attrib {
	struct variable_s variable;
	attrib_t attrib;
};

extern struct variable_type_s fe_attrib_type;

int fe_attrib_init(struct fe_attrib *, const char *, va_list);
int fe_attrib_stringify(struct fe_attrib *, char *, int);



#endif


