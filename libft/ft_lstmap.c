/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstmap.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/20 21:07:53 by ngragas           #+#    #+#             */
/*   Updated: 2020/11/21 21:41:16 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstmap(t_list *lst, void *(*f)(void *), void (*del)(void *))
{
	t_list	*begin;
	t_list	*cur;
	t_list	*last;

	begin = NULL;
	while (lst)
	{
		if (!(cur = ft_lstnew(f(lst->content))))
		{
			while ((cur = begin))
			{
				begin = cur->next;
				del(cur->content);
				free(cur);
			}
			return (NULL);
		}
		if (!begin)
			begin = cur;
		else
			last->next = cur;
		last = cur;
		lst = lst->next;
	}
	return (begin);
}
