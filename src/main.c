/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vchakhno <vchakhno@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 17:05:02 by vchakhno          #+#    #+#             */
/*   Updated: 2023/06/10 15:51:05 by vchakhno         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "heredoc_test.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

bool	run_command(t_command command, pid_t *command_pid)
{
	pid_t	child_pid;

	child_pid = fork();
	if (child_pid == -1)
	{
		fprintf(stderr, "Error: Forking failed\n");
		return (false);
	}
	if (child_pid == 0)
	{
		dup2(command.input_fd, STDIN_FILENO);
		if (execve(command.path, command.args, command.env) == -1)
		{
			fprintf(stderr, "Error: Execve failed. Bad command path?\n");
			exit(1);
		}
	}
	*command_pid = child_pid;
	return (true);
}

bool	wait_command(pid_t command_pid, int *status)
{
	int		wstatus;

	waitpid(command_pid, &wstatus, 0);
	if (!WIFEXITED(wstatus))
		return (false);
	*status = WEXITSTATUS(wstatus);
	return (true);
}

// Surement divisible mais j'ai pas encore trouvé de séparation satisfaisante

bool	run_heredoc(pid_t *heredoc_pid, int *read_fd)
{
	bool				read_status;
	int					pipe_fds[2];
	t_heredoc_content	content;
	pid_t				child_pid;

	if (pipe(pipe_fds) == -1)
	{
		fprintf(stderr, "Error: Heredoc pipe failed\n");
		return (false);
	}
	read_status = heredoc_content_read_all(&content);
	if (read_status)
		child_pid = fork();
	if (!read_status || child_pid == -1)
	{
		heredoc_content_free(&content);
		close(pipe_fds[PIPE_READ_END]);
		close(pipe_fds[PIPE_WRITE_END]);
		return (false);
	}
	if (child_pid == 0)
	{
		close(pipe_fds[PIPE_READ_END]);
		heredoc_content_write_all(&content, pipe_fds[PIPE_WRITE_END]);
		heredoc_content_free(&content);
		close(pipe_fds[PIPE_WRITE_END]);
		exit(0);
	}
	close(pipe_fds[PIPE_WRITE_END]);
	heredoc_content_free(&content);
	*heredoc_pid = child_pid;
	*read_fd = pipe_fds[PIPE_READ_END];
	return (true);
}

// Probablement pas besoin de le wait
// Le heredoc est garanti d'être fini :
// - soit par exit après avoir tout lu
// - soit par SIGPIPE si la commande se termine

void	wait_heredoc(pid_t heredoc_pid)
{
	waitpid(heredoc_pid, NULL, 0);
}

int	main(int argc, char **argv, char **env)
{
	int		read_fd;
	pid_t	heredoc_pid;
	pid_t	command_pid;
	int		status;

	if (argc < 2)
	{
		fprintf(stderr, "Error: Not enough arguments. "
			"Expected: %s [command] [args]\n", argv[0]);
		return (1);
	}
	if (!run_heredoc(&heredoc_pid, &read_fd))
		return (1);
	status = 0;
	if (!run_command((t_command){argv[1], &argv[1], env, read_fd},
		&command_pid))
		status = 1;
	close(read_fd);
	if (status == 0 && !wait_command(command_pid, &status))
		status = 1;
	wait_heredoc(heredoc_pid);
	return (status);
}
