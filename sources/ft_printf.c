/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/03 21:35:04 by ngragas           #+#    #+#             */
/*   Updated: 2021/01/26 18:20:34 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int		ft_printf(const char *format, ...)
{
	t_buf		res;
	va_list		ap;
	const char	*percent_pos;
	int			cur_count;

	res.total_count = 0;
	res.len = 0;
	va_start(ap, format);
	while ((percent_pos = ft_strchr(format, '%')))
	{
		ft_printf_bufcpy(&res, format, percent_pos - format);
		format = percent_pos + 1;
		cur_count = ft_printf_format(&res, ap, &format);
		if (cur_count == -1)
		{
			write(STDOUT_FILENO, res.str, res.len);
			return (-1);
		}
	}
	va_end(ap);
	cur_count = ft_strlen(format);
	ft_printf_bufcpy(&res, format, cur_count);
	write(STDOUT_FILENO, res.str, res.len);
	return (res.total_count);
}

int		ft_printf_format(t_buf *res, va_list ap, const char **format)
{
	t_specs	specs;
	int		count;

	specs = (t_specs){0, 0, 0, 0, '\0'};
	*format = ft_printf_parse_specs(ap, *format, &specs);
	if (specs.type)
		(*format)++;
	count = 0;
	if (specs.type == 's' && specs.len != l)
		count = ft_printf_string(res, va_arg(ap, char *), &specs);
	else if (specs.type == 's' && specs.len == l)
		count = ft_printf_string_utf(res, va_arg(ap, wchar_t *), &specs);
	else if (specs.type == 'd' || specs.type == 'i' || specs.type == 'u')
		count = ft_printf_int(res, ft_printf_int_get(ap, &specs), 10, &specs);
	else if (specs.type == 'o')
		count = ft_printf_int(res, ft_printf_int_get(ap, &specs), 8, &specs);
	else if (specs.type == 'x' || specs.type == 'X' || specs.type == 'p')
		count = ft_printf_int(res, ft_printf_int_get(ap, &specs), 16, &specs);
	else if (specs.type == 'f' || specs.type == 'e' || specs.type == 'g')
		count = ft_printf_float(res, va_arg(ap, double), &specs);
	else if (specs.type == 'n')
		ft_printf_put_count(ap, res->total_count, specs.len);
	else if (specs.type == 'c' || specs.type)
		count = ft_printf_char(res, ap, &specs);
	return (count);
}

void	ft_printf_put_count(va_list ap, int count, char length)
{
	void	*ptr;

	if (length == 0 && (ptr = va_arg(ap, int *)))
		*(int *)ptr = count;
	else if (length == ll && (ptr = va_arg(ap, long long int *)))
		*(long long int *)ptr = count;
	else if (length == l && (ptr = va_arg(ap, long int *)))
		*(long int *)ptr = count;
	else if (length == hh && (ptr = va_arg(ap, char *)))
		*(char *)ptr = (char)count;
	else if (length == h && (ptr = va_arg(ap, short int *)))
		*(short int *)ptr = (short int)count;
}

void	ft_printf_bufcpy(t_buf *buf, const char *str, size_t n)
{
	buf->total_count += n;
	if (buf->len + n > sizeof(buf->str))
	{
		write(STDOUT_FILENO, buf->str, buf->len);
		buf->len = 0;
		if (n > sizeof(buf->str))
		{
			write(STDOUT_FILENO, str, n);
			n = 0;
		}
	}
	while (n--)
		buf->str[buf->len++] = *str++;
}

void	ft_printf_bufset(t_buf *buf, char chr, size_t n)
{
	int	i;

	buf->total_count += n;
	if (buf->len + n >= sizeof(buf->str))
	{
		write(STDOUT_FILENO, buf->str, buf->len);
		buf->len = 0;
		while (n >= sizeof(buf->str))
		{
			i = 0;
			while (i < (int)sizeof(buf->str))
				buf->str[i++] = chr;
			write(STDOUT_FILENO, buf->str, sizeof(buf->str));
			n -= sizeof(buf->str);
		}
	}
	while (n--)
		buf->str[buf->len++] = chr;
}
