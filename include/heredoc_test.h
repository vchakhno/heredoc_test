/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_test.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vchakhno <vchakhno@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 20:27:48 by vchakhno          #+#    #+#             */
/*   Updated: 2023/06/10 02:15:38 by vchakhno         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEREDOC_TEST_H
# define HEREDOC_TEST_H

# include <stdbool.h>

# define PIPE_WRITE_END	1
# define PIPE_READ_END	0

typedef struct s_command
{
	char	*path;
	char	**args;
	char	**env;
	int		input_fd;
}	t_command;

typedef struct s_heredoc_line
{
	char					*line;
	struct s_heredoc_line	*next;
}	t_heredoc_line;

typedef struct s_heredoc_content
{
	t_heredoc_line	*first;
	t_heredoc_line	*last;
}	t_heredoc_content;

bool	heredoc_content_push_line(t_heredoc_content *content, char *line);
bool	heredoc_content_read_all(t_heredoc_content *content);
void	heredoc_content_write_all(t_heredoc_content *content, int write_fd);
void	heredoc_content_free(t_heredoc_content *content);

#endif