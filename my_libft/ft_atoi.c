/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbouizak <mbouizak@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 18:39:13 by mbouizak          #+#    #+#             */
/*   Updated: 2025/08/13 18:39:16 by mbouizak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_atoi(char *str)
{
	long double	nbr;
	int			sign;
	int			i;

	nbr = 0;
	sign = 1;
	i = 0;
	if (str[0] == '-')
	{
		sign = -1;
		i++;
	}
	else if (str[0] == '+')
		i++;
	while ('0' <= str[i] && str[i] <= '9')
	{
		if (nbr > LONG_MAX || (nbr * sign) < LONG_MIN)
		{
			write(2, "minishell: exit: numeric argument required\n", 44);
			return (2);
		}
		nbr = nbr * 10 + str[i] - '0';
		i++;
	}
	return (nbr * sign);
}
