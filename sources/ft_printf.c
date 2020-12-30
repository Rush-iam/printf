/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/03 21:35:04 by ngragas           #+#    #+#             */
/*   Updated: 2020/12/31 00:14:20 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

#include <stdio.h> // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

int	atoi_shift(const char **str)
{
	int	num;

	num = 0;
	while ('0' <= **str && **str <= '9')
	{
		num = num * 10 + (**str - '0');
		(*str)++;
	}
	return (num);
}

const char	*get_specs_2(const char *format, t_specs *specs)
{
	specs->type = *format;
	if (ft_strchr("diuxX", specs->type) && (specs->flags & FLAG_PRECISION))
		specs->flags &= ~FLAG_ZERO;
	if (specs->precision < 0 && (specs->flags & FLAG_PRECISION))
		specs->flags &= ~FLAG_PRECISION;
	if (specs->type && !(specs->flags & FLAG_PRECISION))
	{
		if (ft_strchr("feg", specs->type))
			specs->precision = 6;
		else if (ft_strchr("diuxX", specs->type))
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
	return (format);
}

const char	*get_specs_1(va_list ap, const char *frmt, t_specs *s)
{
	const char	*fields_list = "-+ 0#.lh*123456789";
	const char	*flag;

	while (*frmt && (flag = ft_memchr(fields_list, *frmt, 18)))
	{
		if (flag - fields_list < 5)
		{
			(s->flags |= (1 << (flag - fields_list))) && frmt++;
			continue;
		}
		if (*frmt == '*' || ft_isdigit(*frmt))
			s->width = (*frmt == '*' ? va_arg(ap, int) : atoi_shift(&frmt));
		*frmt == '*' ? frmt++ : 0;
		if (*frmt == '.')
		{
			s->precision = *++frmt == '*' ? va_arg(ap, int) : atoi_shift(&frmt);
			s->precision >= 0 ? s->flags |= FLAG_PRECISION : 0;
			*frmt == '*' ? frmt++ : 0;
		}
		if (*frmt == 'h')
			*++frmt == 'h' ? (s->len |= LEN_HH) && frmt++ : (s->len |= LEN_H);
		else if (*frmt == 'l')
			*++frmt == 'l' ? (s->len |= LEN_LL) && frmt++ : (s->len |= LEN_L);
	}
	return (get_specs_2(frmt, s));
}

static int		reformat(char *res, va_list ap, const char **format, int count)
{
	t_specs	specs;

	specs = (t_specs){0, 0, 0, 0, '\0', NULL};
	*format = get_specs_1(ap, *format, &specs);
	if (**format)
		(*format)++;
	*res = '@';
	count++;

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
//	if (specs.type == 's')
//		count += reformat_string(res, ap, specs);
//	else if (specs.type == 'd' || specs.type == 'u' || specs.type == 'i')
//		count += reformat_integer(res, ap, specs);
//	else if (specs.type == 'p' || specs.type == 'x' || specs.type == 'X')
//		count += reformat_hex(res, ap, specs);
//	else if (specs.type == 'f' || specs.type == 'e' || specs.type == 'g')
//		count += reformat_float(res, ap, specs);
//	else if (specs.type == 'n')
//		put_count(va_arg(ap, int *), count, specs);
//	else if (specs.type == 'c' || specs.type)
//		count += reformat_char(res, ap, specs);
	return (count);
}

int				ft_printf(const char *format, ...)
{
	char 	res[65536];
	int		char_count;
	va_list	ap;

	va_start(ap, format);
	char_count = 0;
	while (*format)
	{
		if (*format == '%')
		{
			format++;
			char_count = reformat(res + char_count, ap, &format, char_count);
		}
		else
			res[char_count++] = *format++;
	}
	write(1, res, char_count);
	va_end(ap);
	return (char_count);
}
