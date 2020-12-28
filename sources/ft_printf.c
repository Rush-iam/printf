/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/03 21:35:04 by ngragas           #+#    #+#             */
/*   Updated: 2020/12/28 23:01:10 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

#include <stdio.h>

static int	is_valid(const char *format)
{
	(void)format;
	return (1);
}

static int	parse_format(va_list ap, char *res, const char *format, int count)
{
	const char	*flag_list = "-+ #0";
	const char	*size_list[] = {"hh", "h", "l", "ll", NULL};
	const char	*types_list = "cspdiuxXnfge";
	(void)flag_list; (void)size_list; (void)types_list;
	(void)res;

	if (!(is_valid(format + count)))
		return (count);

	if (*format == 's')
		printf("arg = %s\n", va_arg(ap, char *));
	else if (*format == 'd')
		printf("arg = %d\n", va_arg(ap, int));
	return (count);
}

int			ft_printf(const char *format, ...)
{
	va_list	ap;
	char 	res[1000];
	int		char_count;

	va_start(ap, format);
	char_count = 0;
	while (*format)
	{
		if (*format != '%')
			res[char_count++] = *format++;
		else if (*++format && *format != '%')
			char_count = parse_format(ap, res, format, char_count);
		else
			res[char_count++] = '%';
	}
	write(1, res, char_count);
	va_end(ap);
	return (char_count);
}
