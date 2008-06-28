/*
 * Module Name:		execute.c
 * System Requirements:	unistd.h
 * Description:		Program Execution Manager
 */

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <stutter/memory.h>
#include <stutter/frontend/curses/execute.h>
#include <stutter/frontend/curses/interface.h>

#define EXECUTE_MAX_PARAMS		64

struct fe_interface_type fe_execute_type = { {
	NULL,
	"execute",
	sizeof(struct fe_interface),
	NULL,
	(object_init_t) fe_execute_init,
	(object_release_t) fe_execute_release },
	(fe_int_read_t) fe_execute_read,
	(fe_int_write_t) fe_execute_write
};

static void fe_execute_sig_child(int);
static int fe_execute_parse_args(char *, char **, int);

int init_execute(void)
{
	signal(SIGCHLD, fe_execute_sig_child);
	return(0);
}

int release_execute(void)
{
	return(0);
}

int fe_execute_init(struct fe_interface *inter, const char *params, va_list va)
{
	char *cmd;
	int bitflags;
	char *argv[EXECUTE_MAX_PARAMS];
	int pid, read_pipe[2], write_pipe[2], error_pipe[2];

	if (fe_interface_init(inter, "", va) < 0)
		return(-1);
	if (params[0] != 's' && (params[1] != 'n' || params[1] != '\0'))
		return(-1);
	cmd = va_arg(va, char *);
	bitflags = va_arg(va, int);
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
		return(-1);
	}

	if ((pid = fork()) == -1) {
		/** The fork failed so close all pipes and return */
		close(read_pipe[0]);
		close(read_pipe[1]);
		close(write_pipe[0]);
		close(write_pipe[1]);
		close(error_pipe[0]);
		close(error_pipe[1]);
		return(-1);
	}
	else if (pid == 0) {
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
		release_interface();

		fe_execute_parse_args(cmd, argv, EXECUTE_MAX_PARAMS);
		execvp(argv[0], argv);
		/** If exec() returns then an error occured.  This will be sent
		    to the parent process through the file descriptor we set up */
		printf("Error occured while executing command\n");
		exit(-1);
	}
	else {
		close(read_pipe[1]);
		close(write_pipe[0]);
		close(error_pipe[1]);
		inter->read = read_pipe[0];
		inter->write = write_pipe[1];
		inter->error = error_pipe[0];
		return(0);
	}
}

void fe_execute_release(struct fe_interface *inter)
{
	fe_interface_release(inter);
}

int fe_execute_read(struct fe_interface *inter, char *buffer, int size)
{
	int i, j;
	fd_set rd;
	struct timeval timeout = { 0, 0 };

	if (i < size) {
		FD_ZERO(&rd);
		FD_SET(inter->read, &rd);
		if (select(inter->read + 1, &rd, NULL, NULL, &timeout)
		    && ((j = read(inter->read, &buffer[i], size - i)) > 0))
			i += j;
		if (j <= 0)
			return(-1);
	}
	return(i);
}

int fe_execute_write(struct fe_interface *inter, const char *data, int len)
{
	int sent, count = 0;

	if (len < 0)
		len = strlen(data);
	do {
		if ((sent = write(inter->write, (void *) data, len)) < 0)
			return(-1);
		else if (!sent)
			return(0);
		count += sent;
	} while (count < len);
	return(count);
}


/*** Local Functions ***/

static void fe_execute_sig_child(int sig)
{
	int status;

	// TODO will this always work or can this cause the progam to lockup?
	wait(&status);
}

static int fe_execute_parse_args(char *cmd, char **argv, int max_args)
{
	int i, j;

	// TODO parse cmd.  should cmd be const?
	argv[0] = cmd;
	for (i = 0, j = 1; cmd[i] != '\0'; i++) {
		if (cmd[i] == ' ') {
			cmd[i++] = '\0';
			while ((cmd[i] != '\0') && (cmd[i] == ' '))
				i++;
			argv[j++] = &cmd[i];
			if (j >= max_args)
				break;
		}
	}
	argv[j] = NULL;
	return(0);
}


