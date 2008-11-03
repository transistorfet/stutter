/*
 * Header Name:		common.h
 * Description:		Common Frontend Header
 */

#ifndef _STUTTER_FRONTEND_COMMON_H
#define _STUTTER_FRONTEND_COMMON_H

#include <stutter/signal.h>
#include <stutter/frontend/common/types.h>
#include <stutter/frontend/common/widget.h>

int init_common_frontend(void);
int release_common_frontend(void);

/* Commands */
int fe_common_cmd_bind(char *, char *);
int fe_common_cmd_clear(char *, char *);
int fe_common_cmd_clearline(char *, char *);
int fe_common_cmd_close(char *, char *);
int fe_common_cmd_context(char *, char *);
int fe_common_cmd_echo(char *, char *);
int fe_common_cmd_insert(char *, char *);
int fe_common_cmd_next(char *, char *);
int fe_common_cmd_parse(char *, char *);
int fe_common_cmd_previous(char *, char *);
int fe_common_cmd_scroll(char *, char *);
int fe_common_cmd_select(char *, char *);
int fe_common_cmd_unbind(char *, char *);
int fe_common_cmd_window(char *, char *);

/* Signal Handlers */
int fe_common_sig_create(void *, struct signal_s *, va_list);
int fe_common_sig_destroy(void *, struct signal_s *, va_list);
int fe_common_sig_print(struct fe_widget *, va_list);

/* Widgets */
extern struct fe_widget_type fe_text_type;
extern struct fe_widget_type fe_frame_type;
extern struct fe_widget_type fe_input_type;
extern struct fe_widget_type fe_window_type;
extern struct fe_widget_type fe_region_type;
extern struct fe_widget_type fe_statusbar_type;

#endif


