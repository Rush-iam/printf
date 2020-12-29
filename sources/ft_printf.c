/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/03 21:35:04 by ngragas           #+#    #+#             */
/*   Updated: 2020/12/29 22:15:52 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

#include <stdio.h>

static t_specs	get_specs_2(const char *format, t_specs specs)
{
	const char	*types_list = "cspdiuxXnfeg";
	const char	*type;

	if (*format == 'h' && --specs.length && *++format == 'h')
		specs.length-- && format++;
	else if (*format == 'l' && ++specs.length && *++format == 'l')
		specs.length++ && format++;
	if ((type = ft_strchr(types_list, *format)))
	{
		specs.type = *type;
		specs.format_end = ++format;
		if (ft_strchr("diuxX", specs.type) && specs.precision)
			specs.flags ^= FLAG_ZERO;
		if (specs.width < 0)
		{
			specs.flags |= FLAG_MINUS;
			specs.width *= -1;
		}
		if (specs.flags & (FLAG_ZERO + FLAG_MINUS))
			specs.flags ^= FLAG_ZERO;
		if (specs.flags & (FLAG_SPACE + FLAG_PLUS))
			specs.flags ^= FLAG_SPACE;
	}
	return (specs);
}

static t_specs	get_specs_1(va_list ap, const char *format)
{
	const char		*flag_list = "-+ 0#";
	unsigned char	flag;
	t_specs			specs;

	specs = (t_specs){0, 0, 0, 0, '\0', NULL};
	while ((flag = (ft_strchr(flag_list, *format) - flag_list)))
		(specs.flags |= 1 << flag) && format++;
	specs.width = (*format == '*' ? va_arg(ap, int) : ft_atoi(format));
	if (*format != '*')
		while (ft_isdigit(*format))
			format++;
	else
		format++;
	if (*format == '.')
	{
		specs.precision = *++format == '*' ? va_arg(ap, int) : ft_atoi(format);
		if (specs.precision < 0)
			specs.precision = 0;
		if (*format != '*')
			while (ft_isdigit(*format))
				format++;
		else
			format++;
	}
	return (get_specs_2(format, specs));
}

static int		reformat(va_list ap, char *res, const char **format, int count)
{
	t_specs	specs;

	specs = get_specs_1(ap, *format);
	if (!(specs.type))
		return (count);
	*format = specs.format_end;
	res[0] = 0;
	if (specs.type == 'c')
		count += reformat_char(res, ap, specs);
	else if (specs.type == 's')
		count += reformat_string(res, ap, specs);
	else if (specs.type == 'd' || specs.type == 'u' || specs.type == 'i')
		count += reformat_integer(res, ap, specs);
	else if (specs.type == 'p' || specs.type == 'x' || specs.type == 'X')
		count += reformat_hex(res, ap, specs);
	else if (specs.type == 'f' || specs.type == 'e' || specs.type == 'g')
		count += reformat_float(res, ap, specs);
	else if (specs.type == 'n')
		put_count(va_arg(ap, int *), count, specs);
	return (count);
}

int				ft_printf(const char *format, ...)
{
	char 	res[4096];
	int		char_count;
	va_list	ap;

	va_start(ap, format);
	char_count = 0;
	while (*format)
	{
		if (*format != '%')
			res[char_count++] = *format++;
		else if (*++format && *format != '%')
			char_count = reformat(ap, res + char_count, &format, char_count);
		else
			res[char_count++] = '%';
	}
	write(1, res, char_count);
	va_end(ap);
	return (char_count);
}
