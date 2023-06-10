/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_content.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vchakhno <vchakhno@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 20:28:29 by vchakhno          #+#    #+#             */
/*   Updated: 2023/06/10 02:52:32 by vchakhno         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "heredoc_test.h"
#include <stdlib.h>
#include <readline/readline.h>
#include <unistd.h>

bool	heredoc_content_push_line(t_heredoc_content *content, char *line)
{
	t_heredoc_line	*node;

	node = malloc(sizeof(t_heredoc_line));
	if (!node)
		return (false);
	node->line = line;
	node->next = NULL;
	if (content->first == NULL)
	{
		content->first = node;
		content->last = node;
	}
	else
	{
		content->last->next = node;
		content->last = node;
	}
	return (true);
}

bool	heredoc_content_read_all(t_heredoc_content *content)
{
	char	*line;

	content->first = NULL;
	content->last = NULL;
	line = readline("heredoc> ");
	while (line)
	{
		if (!heredoc_content_push_line(content, line))
		{
			fprintf(stderr, "Error: Heredoc full\n");
			return (false);
		}
		line = readline("heredoc> ");
	}
	return (true);
}

void	heredoc_content_write_all(t_heredoc_content *content, int write_fd)
{
	t_heredoc_line	*current;
	int				line_len;

	current = content->first;
	while (current)
	{
		line_len = strlen(current->line);
		if (write(write_fd, current->line, line_len) != line_len
			|| write(write_fd, "\n", 1) != 1)
		{
			fprintf(stderr, "Error: Heredoc couldn't be fed to the command\n");
			return ;
		}
		current = current->next;
	}
}

void	heredoc_content_free(t_heredoc_content *content)
{
	t_heredoc_line	*current;
	t_heredoc_line	*tmp;

	current = content->first;
	while (current)
	{
		tmp = current;
		current = current->next;
		free(tmp->line);
		free(tmp);
	}
}
