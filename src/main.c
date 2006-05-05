/*
 * Module Name:		main.c
 * Version:		0.1
 * Module Requirements:	key ; type ; signal ; variable
 * Description:		IRC Client
 */

#include CONFIG_H
#include <stutter/key.h>
#include <stutter/type.h>
#include <stutter/signal.h>
#include <stutter/variable.h>


int init_system(void)
{
	init_signal();
	init_type();
	init_variable();
	init_key();
	return(0);
}

int release_system(void)
{
	release_key();
	release_variable();
	release_type();
	release_signal();
	return(0);
}


