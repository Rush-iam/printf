/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/03 21:35:04 by ngragas           #+#    #+#             */
/*   Updated: 2021/01/07 17:53:08 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

void		ft_printf_bufcpy(t_buf *buf, const char *src, size_t n)
{
	if (buf->len + n > sizeof(buf->str))
	{
		write(STDOUT_FILENO, buf->str, buf->len);
		buf->len = 0;
		if (n > sizeof(buf->str))
		{
			write(STDOUT_FILENO, src, n);
			n = 0;
		}
	}
	while (n--)
		buf->str[buf->len++] = *src++;
}

void		ft_printf_bufset(t_buf *buf, char c, size_t n)
{
	int	i;

	if (buf->len + n >= sizeof(buf->str))
	{
		write(STDOUT_FILENO, buf->str, buf->len);
		buf->len = 0;
		while (n >= sizeof(buf->str))
		{
			i = 0;
			while (i < (int)sizeof(buf->str))
				buf->str[i++] = c;
			write(STDOUT_FILENO, buf->str, sizeof(buf->str));
			n -= sizeof(buf->str);
		}
	}
	while (n--)
		buf->str[buf->len++] = c;
}

int			ft_printf(const char *format, ...)
{
	int		total_count;
	int		cur_count;
	t_buf	res;
	va_list	ap;
	char	*percent_pos;

	total_count = 0;
	res.len = 0;
	va_start(ap, format);
	while ((percent_pos = ft_strchr(format, '%')))
	{
		total_count += percent_pos - format;
		ft_printf_bufcpy(&res, format, percent_pos - format);
		format = percent_pos + 1;
		cur_count = ft_printf_format(&res, ap, &format);
		if (cur_count == -1)
			return (-1);
		total_count += cur_count;
	}
	va_end(ap);
	cur_count = ft_strlen(format);
	ft_printf_bufcpy(&res, format, cur_count);
	total_count += cur_count;
	write(STDOUT_FILENO, res.str, res.len);
	return (total_count);
}

int			ft_printf_format(t_buf *res, va_list ap, const char **format)
{
	t_specs	specs;
	int		count;

	specs = (t_specs){0, 0, 0, 0, '\0'};
	*format = ft_printf_parse_specs_1(ap, *format, &specs);
	if (specs.type)
		(*format)++;

#include <stdio.h> //!!!!!!!!
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

	count = 0;
	if (specs.type == 's' && specs.len != LEN_L)
		count = ft_printf_string(res, va_arg(ap, char *), &specs);
	else if (specs.type == 's' && specs.len == LEN_L)
		count = ft_printf_string_utf(res, va_arg(ap, wchar_t *), &specs);
	else if (specs.type == 'd' || specs.type == 'i' || specs.type == 'u')
		count = ft_printf_int(res, ft_printf_int_get(ap, &specs), 10, &specs);
	else if (specs.type == 'x' || specs.type == 'X' || specs.type == 'p')
		count = ft_printf_int(res, ft_printf_int_get(ap, &specs), 16, &specs);
//	else if (specs.type == 'f' || specs.type == 'e' || specs.type == 'g')
//		count += reformat_float(res, ap, specs);
//	else if (specs.type == 'n')
//		put_count(va_arg(ap, int *), count, specs);
	else if (specs.type == 'c' || specs.type)
		count = ft_printf_char(res, ap, &specs);
	return (count);
}

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

const char	*ft_printf_parse_specs_2(const char *fstr, t_specs *specs)
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
