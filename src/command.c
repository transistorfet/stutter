/*
 * Module Name:		command.c
 * Version:		0.1
 * Module Requirements:	frontend ; list ; memory ; callback
 * Description:		Command Manager
 */


#include <string.h>

#include CONFIG_H
#include FRONTEND_H
#include <error.h>
#include <command.h>
#include <nit/list.h>
#include <nit/memory.h>
#include <nit/callback.h>

struct command_s {
	char *name;
	struct callback_s *execute;
};

struct list_s *command_list = NULL;

static int compare_command(struct command_s *, char *);
static void destroy_command(struct command_s *);

int init_command(void)
{
	if (!(command_list = create_list(0, (compare_t) compare_command, (destroy_t) destroy_callback)))
		return(-1);
	return(0);
}

int release_command(void)
{
	destroy_list(command_list);
	return(0);
}

/**
 * Add a command of the given name and calls the given callback
 * function passing it the given pointer.  If a command of the
 * given name already exists, the command is replaced.
 */
int add_command(char *name, callback_t callback, void *ptr)
{
	struct command_s *command;

	if (!name)
		return(-1);
	// TODO check for replacing
	if (!(command = memory_alloc(sizeof(struct command_s) + strlen(name) + 1)))
		return(-1);
	command->name = (char *) (((unsigned int) command) + sizeof(struct command_s));
	strcpy(command->name, name);
	command->execute = create_callback(0, 0, NULL, callback, ptr);

	return(list_add(command_list, command));
}

/**
 * The command of the given name is deleted from the command
 * list and all associated memory is freed.
 */
int remove_command(char *name)
{
	return(list_delete(command_list, name));
}

/**
 * Looks up the given command name and calls the associated
 * callback passing the given pointer as the invocation-supplied
 * parameter (the second parameter).  If  the command is not
 * found then a -1 is returned, otherwise the number returned
 * by the command function is returned.
 */
int evaluate_command(char *name, void *ptr)
{
	struct command_s *command;

	list_clear_current(command_list);
	if (command = list_find(command_list, name, 0))
		return(execute_callback(command->execute, ptr));
//	else if (command = list_find(command_list, name, 0))
//		return(execute_callback(command->execute, ptr));
	return(ERR_COMMAND_NOT_FOUND);
}

/*** Local Functions ***/

/**
 * Compare the name of the given command with the given name and
 * return 0 if they match or are both NULL, otherwise return a
 * non-zero value.
 */
static int compare_command(struct command_s *command, char *name)
{
	if (!name) {
		if (!command->name)
			return(0);
		return(1);
	}
	return(strcmp(command->name, name));
}

/**
 * Free all resources used by the given command struct.
 */
static void destroy_command(struct command_s *command)
{
	destroy_callback(command->execute);
	memory_free(command);
}


