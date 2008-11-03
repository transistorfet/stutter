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

	add_signal(VARIABLE_S(signal_table), "output");
	add_signal(VARIABLE_S(signal_table), "output.create");
	add_signal(VARIABLE_S(signal_table), "output.destroy");
	add_signal(VARIABLE_S(signal_table), "output.status");
	add_signal(VARIABLE_S(signal_table), "output.error");
	return(0);
}

int release_system(void)
{
	release_signal();
	release_variable();
	release_object();
	return(0);
}


