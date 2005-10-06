/*
 * Module Name:		status.c
 * Version:		0.1
 * Module Requirements:	frontend ; modirc
 * Description:		IRC Status Bar
 */

#include CONFIG_H
#include <frontend.h>
#include <nit/string.h>
#include <modules/irc.h>

/**
 * Status Bar Refresh Function.  Redraw the status bar in the given frame
 * using the information from the given irc structure.
 */
int irc_status_bar(char *env, void *widget)
{
	char *str;

	if (!(str = irc_format_msg(NULL, IRC_STATUS_BAR)))
		return(-1);
	if (strlen(str) > fe_get_width(widget))
		str[fe_get_width(widget)] = '\0';
	fe_clear(widget);
	fe_print(widget, str);
	return(0);
}


