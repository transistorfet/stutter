/*
 * Module Name:		init.c
 * Description:		System Initialization
 */

#include CONFIG_H
#include <stutter/init.h>
#include <stutter/object.h>
#include <stutter/signal.h>
#include <stutter/variable.h>

int init_system(void)
{
	init_object();
	init_variable();
	init_signal();
	return(0);
}

int release_system(void)
{
	release_signal();
	release_variable();
	release_object();
	return(0);
}


