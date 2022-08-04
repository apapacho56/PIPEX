/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aparedes <aparedes@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/04 16:49:54 by aparedes          #+#    #+#             */
/*   Updated: 2022/08/04 16:50:59 by aparedes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/pipex.h"

void	execute_cmd1(t_pipex *pipex)
{
	dup2(pipex->fd1, 0);
	dup2(pipex->fd[1], 1);
	close(pipex->fd[1]);
	close(pipex->fd[0]);
	execve(pipex->cmd1, pipex->cmd1_args, pipex->env);
}

void	execute_cmd2(t_pipex *pipex)
{
	dup2(pipex->fd2, 1);
	dup2(pipex->fd[0], 0);
	close(pipex->fd[0]);
	execve(pipex->cmd2, pipex->cmd2_args, pipex->env);
}

void	ft_pipex(t_pipex *pipex)
{
	int	pid;

	pid = fork();
	if (pid == -1)
		error("Error Fork", 1);
	else if (pid == 0)
		execute_cmd1(pipex);
	close(pipex->fd[1]);
	pid = fork();
	if (pid == 0)
		execute_cmd2(pipex);
	close(pipex->fd[0]);
	waitpid(pipex->fd[0], &(pipex->fd_status), 0);
	waitpid(pipex->fd[1], &(pipex->fd_status), 0);
}

void	check_pipex(t_pipex **pipex)
{
	if ((*pipex)->fd1 < 0)
		error(strerror(errno), 0);
	if ((*pipex)->fd2 < 0)
		error(strerror(errno), 0);
	(*pipex)->cmd1_args = ft_split((*pipex)->cmd1, ' ');
	get_cmd_path(pipex, 1);
	(*pipex)->cmd2_args = ft_split((*pipex)->cmd2, ' ');
	get_cmd_path(pipex, 2);
}

int	main(int argc, char *argv[], char **env)
{
	t_pipex	*pipex;

	pipex = NULL;
	if (argc == 5)
	{
		pipex = (t_pipex *)malloc(sizeof(t_pipex));
		if (pipe(pipex->fd) == -1)
			error("Pipe error.", 1);
		pipex->fd1 = open(argv[1], O_RDONLY);
		pipex->fd2 = open(argv[4], O_WRONLY | O_CREAT | O_TRUNC, 0666);
		pipex->env = env;
		pipex->cmd1 = argv[2];
		pipex->cmd2 = argv[3];
		check_pipex(&pipex);
		ft_pipex(pipex);
		close(pipex->fd1);
		close(pipex->fd2);
	}
	else
		error("Invalid number of arguments", 1);
	if (WIFEXITED(pipex->fd_status))
		return (WEXITSTATUS(pipex->fd_status));
	return (0);
}
