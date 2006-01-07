/*
 * Header Name:		statusbar.h
 * Version:		0.1
 * Description:		Status Bar Widget Header
 */

#ifndef _SIMPLE_STATUSBAR_H
#define _SIMPLE_STATUSBAR_H

#include "../common/curses/screen.h"

struct statusbar_s {
	int height;
	struct variable_s *status;
};

struct statusbar_s *create_statusbar(int, char *, char *);
int destroy_statusbar(struct statusbar_s *);
int refresh_statusbar(struct statusbar_s *, struct screen_s *);

#endif


