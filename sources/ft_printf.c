/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/03 21:35:04 by ngragas           #+#    #+#             */
/*   Updated: 2020/12/30 22:52:57 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

#include <stdio.h> // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

int	ft_atoi_shift(const char **src)
{
	const char	*str = *src;
	int			num;

	num = 0;
	while ('0' <= *str && *str <= '9')
		num = num * 10 + (*str++ - '0');
	*src = str;
	return (num);
}

static t_specs	get_specs_2(const char *format, t_specs specs)
{
	if (*format == 'h' && --specs.length && *++format == 'h')
		specs.length-- && format++;
	else if (*format == 'l' && ++specs.length && *++format == 'l')
		specs.length++ && format++;
	specs.type = *format;
	specs.format_end = format;
	if (ft_strchr("diuxX", specs.type) && (specs.flags & FLAG_PRECISION))
		specs.flags &= ~FLAG_ZERO;
	if (specs.type && !(specs.flags & FLAG_PRECISION))
	{
		if (ft_strchr("feg", specs.type))
			specs.precision = 6;
		else if (ft_strchr("feg", specs.type))
			specs.precision = 1;
	}
	if (specs.width < 0)
	{
		specs.flags |= FLAG_MINUS;
		specs.width *= -1;
	}
	if (specs.flags & FLAG_MINUS)
		specs.flags &= ~FLAG_ZERO;
	if (specs.flags & FLAG_PLUS)
		specs.flags &= ~FLAG_SPACE;
	return (specs);
}

static t_specs	get_specs_1(va_list ap, const char *format)
{
	const char	*flag_list = "-+ 0#";
	const char	*flag;
	t_specs		specs;

	specs = (t_specs){0, 0, 1, 0, '\0', NULL};
	while ((flag = ft_strchr(flag_list, *++format)))
		specs.flags |= (1 << (flag - flag_list));
	specs.width = (*format == '*' ? va_arg(ap, int) : ft_atoi_shift(&format));
	if (*format == '*')
		format++;
	if (*format == '.')
	{
		specs.precision = *++format == '*' ? \
									va_arg(ap, int) : ft_atoi_shift(&format);
		if (specs.precision >= 0)
			specs.flags |= FLAG_PRECISION;
		if (*format == '*')
			format++;
	}
	return (get_specs_2(format, specs));
}

static int		reformat(char *res, va_list ap, const char **format, int count)
{
	t_specs	specs;

	specs = get_specs_1(ap, *format);
	*format = specs.format_end;
	if (**format)
		(*format)++;
	*res = '@';
	count++;

	printf("\nspecs.type = %c", specs.type);
	printf("\nspecs.width = %d", specs.width);
	printf("\nspecs.precision = %d", specs.precision);
	printf("\nspecs.length = %hhd", specs.length);
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
		if (*format != '%')
			res[char_count++] = *format++;
		else if (*format)
			char_count = reformat(res + char_count, ap, &format, char_count);
	}
	write(1, res, char_count);
	va_end(ap);
	return (char_count);
}
