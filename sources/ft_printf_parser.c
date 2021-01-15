/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_parser.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/07 17:58:31 by ngragas           #+#    #+#             */
/*   Updated: 2021/01/15 19:09:59 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

const char	*ft_printf_parse_specs_1(va_list ap, const char *fstr, t_specs *s)
{
	const char	fields_list[] = "-+ 0#.lh*123456789";
	const char	*flag;

	while (*fstr && (flag = ft_memchr(fields_list, *fstr, sizeof(fields_list))))
	{
		if (flag - fields_list < 5 && fstr++)
		{
			s->flags = (char)(s->flags | 1 << (flag - fields_list));
			continue;
		}
		if (*fstr == '*' || ft_isdigit(*fstr))
			s->width = (*fstr++ == '*') ?
						va_arg(ap, int) : ft_printf_parse_atoi(&fstr);
		if (*fstr == '.' && fstr++)
		{
			s->prec = (*fstr++ == '*') ?
					  va_arg(ap, int) : ft_printf_parse_atoi(&fstr);
			s->prec >= 0 ? s->flags |= FLAG_PRECISION : 0;
		}
		if (*fstr == 'h')
			*++fstr == 'h' ? (s->len |= LEN_HH) && fstr++ : (s->len |= LEN_H);
		else if (*fstr == 'l')
			*++fstr == 'l' ? (s->len |= LEN_LL) && fstr++ : (s->len |= LEN_L);
	}
	return (ft_printf_parse_specs_2(fstr, s));
}

const char	*ft_printf_parse_specs_2(const char *fstr, t_specs *specs)
{
	specs->type = *fstr;
	if (specs->type == '\0')
		return (fstr);
	if (specs->type == 'p')
		specs->len = LEN_L;
	if (ft_strchr("diuxX", specs->type) && (specs->flags & FLAG_PRECISION))
		specs->flags &= ~FLAG_ZERO;
	if (specs->prec < 0 || (specs->flags & FLAG_PRECISION) == 0)
	{
		specs->flags &= ~FLAG_PRECISION;
		if (ft_strchr("feg", specs->type))
			specs->prec = 6;
		else if (ft_strchr("diuxXp", specs->type))
			specs->prec = 1;
	}
	if (specs->type == 'g' && specs->prec > 0)
		specs->prec--;
	if (specs->width < 0)
		specs->flags |= FLAG_MINUS && (specs->width *= -1);
	if (specs->flags & FLAG_MINUS)
		specs->flags &= ~FLAG_ZERO;
	if (specs->flags & FLAG_PLUS)
		specs->flags &= ~FLAG_SPACE;
	return (fstr);
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
