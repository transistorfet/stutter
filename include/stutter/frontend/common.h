/*
 * Header Name:		common.h
 * Version:		0.1
 * Description:		Common Frontend Header
 */

#ifndef _STUTTER_FRONTEND_COMMON_H
#define _STUTTER_FRONTEND_COMMON_H

#include <stutter/type.h>

/* Commands */
int fe_common_cmd_insert(char *, char *);

/* Types */
struct type_s *fe_common_load_attrib(void);
struct type_s *fe_common_load_colour(void);
struct type_s *fe_common_load_command(void);
struct type_s *fe_common_load_hashattrib(void);
struct type_s *fe_common_load_style(void);

/* Widgets */
extern struct widget_type_s text_type;
extern struct widget_type_s frame_type;
extern struct widget_type_s input_type;
extern struct widget_type_s window_type;
extern struct widget_type_s region_type;
extern struct widget_type_s statusbar_type;

#endif


