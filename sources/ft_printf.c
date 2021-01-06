/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/03 21:35:04 by ngragas           #+#    #+#             */
/*   Updated: 2021/01/06 20:55:18 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int					ft_printf(const char *format, ...)
{
	char	res[65536];
	int		char_count;
	va_list	ap;

	va_start(ap, format);
	char_count = 0;
	while (*format)
	{
		if (*format == '%')
		{
			format++;
			char_count = ft_printf_format(res + char_count, ap, &format,
											char_count);
			if (char_count == -1)
				return (-1);
		}
		else
			res[char_count++] = *format++;
	}
	write(1, res, char_count);
	va_end(ap);
	return (char_count);
}

int					ft_printf_format(char *res, va_list ap, const char **fstr,
										int count)
{
	t_specs	specs;
	int		f_count;

	specs = (t_specs){0, 0, 0, 0, '\0'};
	*fstr = ft_printf_parse_specs_1(ap, *fstr, &specs);
	if (specs.type)
		(*fstr)++;

#include <stdio.h>
	printf("\nspecs.type = %c", specs.type);
	printf("\nspecs.width = %d", specs.width);
	printf("\nspecs.precision = %d", specs.precision);
	printf("\nspecs.length = %hhd", specs.len);
	printf("\nspecs.flags = \"");
	if (specs.flags & FLAG_MINUS) printf("-");
	if (specs.flags & FLAG_PLUS) printf("+");
	if (specs.flags & FLAG_SPACE) printf(" ");
	if (specs.flags & FLAG_ZERO) printf("0");
	if (specs.flags & FLAG_HASH) printf("#");
	if (specs.flags & FLAG_PRECISION) printf(".");
	printf("\"\n");

	f_count = 0;
	if (specs.type == 's' && specs.len != LEN_L)
		f_count = ft_printf_string(res, va_arg(ap, char *), &specs);
	else if (specs.type == 's' && specs.len == LEN_L)
		f_count = ft_printf_string_utf(res, va_arg(ap, wchar_t *), &specs);
	else if (specs.type == 'd' || specs.type == 'i' || specs.type == 'u')
		f_count = ft_printf_int(res, ft_printf_int_get(ap, &specs), 10, &specs);
	else if (specs.type == 'x' || specs.type == 'X' || specs.type == 'p')
		f_count = ft_printf_int(res, ft_printf_int_get(ap, &specs), 16, &specs);
//	else if (specs.type == 'f' || specs.type == 'e' || specs.type == 'g')
//		count += reformat_float(res, ap, specs);
//	else if (specs.type == 'n')
//		put_count(va_arg(ap, int *), count, specs);
	else if (specs.type == 'c' || specs.type)
		f_count = ft_printf_char(res, ap, &specs);
	return (f_count == -1 ? -1 : count + f_count);
}

const char			*ft_printf_parse_specs_1(va_list ap, const char *fstr,
											t_specs *s)
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
			s->precision = (*fstr++ == '*') ?
						va_arg(ap, int) : ft_printf_parse_atoi(&fstr);
			s->precision >= 0 ? s->flags |= FLAG_PRECISION : 0;
		}
		if (*fstr == 'h')
			*++fstr == 'h' ? (s->len |= LEN_HH) && fstr++ : (s->len |= LEN_H);
		else if (*fstr == 'l')
			*++fstr == 'l' ? (s->len |= LEN_LL) && fstr++ : (s->len |= LEN_L);
	}
	return (ft_printf_parse_specs_2(fstr, s));
}

const char			*ft_printf_parse_specs_2(const char *fstr, t_specs *specs)
{
	specs->type = *fstr;
	if (specs->type == '\0')
		return (fstr);
	if (specs->type == 'p')
		specs->len = LEN_L;
	if (ft_strchr("diuxX", specs->type) && (specs->flags & FLAG_PRECISION))
		specs->flags &= ~FLAG_ZERO;
	if (specs->precision < 0 || (specs->flags & FLAG_PRECISION) == 0)
	{
		specs->flags &= ~FLAG_PRECISION;
		if (ft_strchr("feg", specs->type))
			specs->precision = 6;
		else if (ft_strchr("diuxXp", specs->type))
			specs->precision = 1;
	}
	if (specs->width < 0)
	{
		specs->flags |= FLAG_MINUS;
		specs->width *= -1;
	}
	if (specs->flags & FLAG_MINUS)
		specs->flags &= ~FLAG_ZERO;
	if (specs->flags & FLAG_PLUS)
		specs->flags &= ~FLAG_SPACE;
	return (fstr);
}

int					ft_printf_parse_atoi(const char **str)
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
