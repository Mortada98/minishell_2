/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   true_or_false.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: helfatih <helfatih@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/09 14:13:25 by helfatih          #+#    #+#             */
/*   Updated: 2025/08/09 16:31:39 by helfatih         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

bool	*init_bool(void)
{
	static bool	var = false;

	return (&var);
}

void	set_bool(bool val)
{
	bool	*status;

	status = init_bool();
	*status = val;
}

bool	get_bool(void)
{
	return (*init_bool());
}
