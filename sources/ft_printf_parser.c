/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_parser.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/07 17:58:31 by ngragas           #+#    #+#             */
/*   Updated: 2021/01/18 21:37:26 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

const char	*ft_printf_parse_specs(va_list ap, const char *frmt, t_specs *specs)
{
	const char	fields_list[] = "-+ 0#.lh*123456789";
	const char	*flag;

	while (*frmt && (flag = ft_memchr(fields_list, *frmt, sizeof(fields_list))))
	{
		if (flag - fields_list < 5 && frmt++)
		{
			specs->flags = (char)(specs->flags | 1 << (flag - fields_list));
			continue;
		}
		if (*frmt == '*' || ft_isdigit(*frmt))
			specs->width = (*frmt++ == '*') ?
								va_arg(ap, int) : ft_printf_parse_atoi(&frmt);
		if (*frmt == '.' && frmt++)
		{
			specs->prec = (*frmt++ == '*') ?
								va_arg(ap, int) : ft_printf_parse_atoi(&frmt);
			specs->prec >= 0 ? specs->flags |= flag_precision : 0;
		}
		if (*frmt == 'h')
			*++frmt == 'h' ? (specs->len |= hh) && frmt++ : (specs->len |= h);
		else if (*frmt == 'l')
			*++frmt == 'l' ? (specs->len |= ll) && frmt++ : (specs->len |= l);
	}
	return (ft_printf_parse_refine(frmt, specs));
}

const char	*ft_printf_parse_refine(const char *frmt, t_specs *specs)
{
	specs->type = *frmt;
	if (specs->type == '\0')
		return (frmt);
	if (specs->type == 'p')
		specs->len = l;
	if (ft_strchr("diuxXpo", specs->type) && (specs->flags & flag_precision))
		specs->flags &= ~flag_zero;
	if (specs->prec < 0 || (specs->flags & flag_precision) == 0)
	{
		specs->flags &= ~flag_precision;
		if (ft_strchr("feg", specs->type))
			specs->prec = 6;
		else if (ft_strchr("diuxXpo", specs->type))
			specs->prec = 1;
	}
	if (specs->type == 'g' && specs->prec > 0)
		specs->prec--;
	if (specs->width < 0)
		specs->flags |= flag_minus && (specs->width *= -1);
	if (specs->flags & flag_minus)
		specs->flags &= ~flag_zero;
	if (specs->flags & flag_plus)
		specs->flags &= ~flag_space;
	return (frmt);
}

int			ft_printf_parse_atoi(const char **str)
{
	int	num;

	num = 0;
	(*str)--;
	while ('0' <= **str && **str <= '9')
	{
		num = num * 10 + (**str - '0');
		(*str)++;
	}
	return (num);
}
