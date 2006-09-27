/*
 * Module Name:		execute.c
 * Version:		0.1
 * System Requirements:	unistd.h
 * Description:		Program Execution Manager
 */


#include <string.h>
#include <signal.h>
#include <unistd.h>

#include <stutter/lib/memory.h>
#include "desc.h"

#define EXECUTE_MAX_PARAMS		64

typedef struct fe_descriptor_s *fe_execute_t;

static struct fe_descriptor_list_s *exec_list;

static void fe_exec_free_handle(struct fe_descriptor_s *);
static void fe_exec_sig_child(int);

int init_execute(void)
{
	if (exec_list)
		return(1);
	if (!(exec_list = fe_desc_create_list((destroy_t) fe_exec_free_handle)))
		return(-1);
	signal(SIGCHLD, fe_exec_sig_child);
	return(0);
}

int release_execute(void)
{
	if (!exec_list)
		return(1);
	fe_desc_destroy_list(exec_list);
	return(0);
}

/**
 * Execute the given command and return a reference to the running program.
 */
fe_execute_t fe_execute_open(char *cmd, int bitflags)
{
	int i, j;
	struct fe_descriptor_s *desc;
	char *argv[EXECUTE_MAX_PARAMS];
	int pid, read_pipe[2], write_pipe[2], error_pipe[2];

	if (pipe(read_pipe) || pipe(write_pipe) || pipe(error_pipe)) {
		/** An error occurred when creating the pipes so close any that
		    where successfully opened and return */
		if (read_pipe[0] != -1) {
			close(read_pipe[0]);
			close(read_pipe[1]);
		}
		if (write_pipe[0] != -1) {
			close(write_pipe[0]);
			close(write_pipe[1]);
		}
		if (error_pipe[0] != -1) {
			close(error_pipe[0]);
			close(error_pipe[1]);
		}
		return(NULL);
	}

	if ((pid = fork()) == -1) {
		/** The fork failed so close all pipes and return */
		close(read_pipe[0]);
		close(read_pipe[1]);
		close(write_pipe[0]);
		close(write_pipe[1]);
		close(error_pipe[0]);
		close(error_pipe[1]);
		return(NULL);
	}
	else if (pid == 0) {
		extern int release_desc(void);

		/** Set the stdio descriptors to be the appropriate pipes
		    that we just created */
		dup2(read_pipe[1], 1);
		dup2(write_pipe[0], 0);
		dup2(error_pipe[1], 2);
		/** Close all open descriptors */
		close(read_pipe[0]);
		close(read_pipe[1]);
		close(write_pipe[0]);
		close(write_pipe[1]);
		close(error_pipe[0]);
		close(error_pipe[1]);
		release_desc();

		// TODO parse cmd.  should cmd be const?
		// HOLY CRAP does the memory for the params need to be alloc'd so that
		//		it is around later??
		argv[0] = cmd;
		for (i = 0, j = 1;cmd[i] != '\0';i++) {
			if (cmd[i] == ' ') {
				cmd[i++] = '\0';
				while ((cmd[i] != '\0') && (cmd[i] == ' '))
					i++;
				argv[j++] = &cmd[i];
			}
		}
		argv[j] = NULL;
		execvp(argv[0], argv);
		/** If exec() returns then an error occured */
		printf("Error occured while executing command\n");
		exit(-1);
	}
	else {
		if (!(desc = fe_desc_create(exec_list, 0)))
			return(NULL);
		close(read_pipe[1]);
		close(write_pipe[0]);
		close(error_pipe[1]);
		desc->read = read_pipe[0];
		desc->write = write_pipe[1];
		desc->error = error_pipe[0];
		return(desc);
	}
}

/**
 * Close the given program reference.
 */
void fe_execute_close(fe_execute_t desc)
{
	if (!desc)
		return;
	fe_desc_destroy(exec_list, desc);
}

/**
 * Send the string of length len to the given process and
 * return the number of bytes written or -1 on error.
 */
int fe_execute_send(fe_execute_t desc, char *buffer, int len)
{
	int sent, count = 0;

	if (!desc)
		return(0);
	do {
		if ((sent = write(desc->read, (void *) buffer, len)) < 0)
			return(-1);
		else if (!sent)
			return(0);
		count += sent;
	} while (count < len);
	return(count);
}

/**
 * Receive the given number of bytes, store them in the given buffer
 * and return the number of bytes read or -1 on error or disconnect.
 */ 
int fe_execute_receive(fe_execute_t desc, char *buffer, int len)
{
	int i, j;
	fd_set rd;
	struct timeval timeout = { 0, 0 };

	if (!desc)
		return(-1);

	len--;
	for (i = 0;i < len;i++) {
		if (desc->read_pos >= desc->read_length)
			break;
		buffer[i] = desc->read_buffer[desc->read_pos++];
	}

	if (i < len) {
		FD_ZERO(&rd);
		FD_SET(desc->read, &rd);
		if (select(desc->read + 1, &rd, NULL, NULL, &timeout) && ((j = read(desc->read, &buffer[i], len - i)) > 0))
			i += j;
		if (j <= 0)
			return(-1);
	}

	buffer[i] = '\0';
	return(i);
}

/**
 * Receive a string from the network connection up to a maximum of
 * len-1 (a null char is appended) and return the number of bytes
 * read or -1 on error or disconnect.
 */ 
int fe_execute_receive_str(fe_execute_t desc, char *buffer, int len, char ch)
{
	int i;
	fd_set rd;
	struct timeval timeout = { 0, 0 };

	if (!desc)
		return(-1);

	len--;
	for (i = 0;i < len;i++) {
		if (desc->read_pos >= desc->read_length) {
			FD_ZERO(&rd);
			FD_SET(desc->read, &rd);
			// TODO if either of these fail, we return a partial message which the server
			//	doesn't check for and can't deal with so what we really need to do is
			//	copy what we've read (in buffer) to the buffer and return 0 but what
			//	if the buffer is bigger than the buffer?
			// TODO what about a socket error of some sorts too.  That doesn't really count
			//	as a buffer that hasn't been fully recieved.  We could send a signal to report
			//	the error somehow (which raises the question of should we do (and how would
			//	we do) socket specific signals.
			if (!select(desc->read + 1, &rd, NULL, NULL, &timeout)) {
				buffer[i + 1] = '\0';
				return(i);
			}
			if ((desc->read_length = read(desc->read, desc->read_buffer, DESC_READ_BUFFER)) <= 0)
				return(-1);
			desc->read_pos = 0;
		}
		buffer[i] = desc->read_buffer[desc->read_pos++];
		if (buffer[i] == ch)
			break;
	}

	buffer[i + 1] = '\0';
	return(i);
}

/*** Local Functions ***/

static void fe_exec_free_handle(struct fe_descriptor_s *desc)
{
	if (desc->read != -1)
		close(desc->read);
	if (desc->write != -1)
		close(desc->write);
	if (desc->error != -1)
		close(desc->error);
}

static void fe_exec_sig_child(int sig)
{
	int status;

	// TODO will this always work or can this cause the progam to lockup?
	wait(&status);
}


