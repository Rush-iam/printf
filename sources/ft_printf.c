/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/03 21:35:04 by ngragas           #+#    #+#             */
/*   Updated: 2021/01/04 22:58:34 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

#include <stdio.h> // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

size_t			ft_wstrlen(const wchar_t *s, ssize_t n)
{
	int		char_size;
	size_t	count;

	count = 0;
	while (*s && (n == -1 || count < n))
	{
		if (*s >= 65536)
			char_size = 4;
		else if (*s >= 2048)
			char_size = 3;
		else if (*s >= 128)
			char_size = 2;
		else
			char_size = 1;
		if (n != -1 && count + char_size > n)
			break ;
		count += char_size;
		s++;
	}
	return (count);
}

int				ft_wchrtoutf8(char *dst, const wchar_t src)
{
	int	bytes;

	bytes = 1;
	if (src < 128)
		*dst = (char)src;
	else if (src < 2048)
	{
		dst[0] = (char)(0b11000000 | src >> 6);
		(dst[1] = (char)(0b10000000 | src & 0b111111)) && (bytes++);
	}
	else if (src < 65536)
	{
		dst[0] = (char)(0b11100000 | src >> 12);
		dst[1] = (char)(0b10000000 | src >> 6 & 0b111111);
		(dst[2] = (char)(0b10000000 | src & 0b111111)) && (bytes += 2);
	}
	else if (src < 1114112)
	{
		dst[0] = (char)(0b11110000 | src >> 18);
		dst[1] = (char)(0b10000000 | src >> 12 & 0b111111);
		dst[2] = (char)(0b10000000 | src >> 6 & 0b111111);
		(dst[3] = (char)(0b10000000 | src & 0b111111)) && (bytes += 3);
	}
	return (bytes);
}

size_t			ft_wstrtoutf8(char *dst, const wchar_t *src, size_t n)
{
	int		char_size;
	size_t	count;

	count = 0;
	while (*src && count < n)
	{
		if (*src >= 65536)
			char_size = 4;
		else if (*src >= 2048)
			char_size = 3;
		else if (*src >= 128)
			char_size = 2;
		else
			char_size = 1;
		if (count + char_size > n)
			break ;
		count += char_size;
		dst += ft_wchrtoutf8(dst, *src);
		src++;
	}
	*dst = '\0';
	return (count);
}

size_t			reformat_string_utf8(char *res, wchar_t *string, t_specs specs)
{
	const size_t	src_len = ft_wstrlen(string, -1);
	size_t			dst_len;
	size_t			real_len;

	if ((specs.flags & FLAG_PRECISION) && specs.precision < src_len)
		dst_len = ft_wstrlen(string, specs.precision);
	else
		dst_len = src_len;
	real_len = (specs.width > dst_len ? specs.width : dst_len);
	if (specs.flags & FLAG_MINUS)
	{
		ft_wstrtoutf8(res, string, dst_len);
		res += dst_len;
		ft_memset(res, ' ', real_len - dst_len);
	}
	else
	{
		ft_memset(res, ' ', real_len - dst_len);
		res += real_len - dst_len;
		ft_wstrtoutf8(res, string, dst_len);
	}
	return (real_len);
}

size_t			reformat_string(char *res, char *string, t_specs specs)
{
	const size_t	src_len = ft_strlen(string);
	size_t			dst_len;
	size_t			real_len;

	if ((specs.flags & FLAG_PRECISION) && specs.precision < src_len)
		dst_len = specs.precision;
	else
		dst_len = src_len;
	real_len = (specs.width > dst_len ? specs.width : dst_len);
	if (specs.flags & FLAG_MINUS)
	{
		ft_memcpy(res, string, dst_len);
		res += dst_len;
		ft_memset(res, ' ', real_len - dst_len);
	}
	else
	{
		ft_memset(res, ' ', real_len - dst_len);
		res += real_len - dst_len;
		ft_memcpy(res, string, dst_len);
	}
	return (real_len);
}

static int		atoi_shift(const char **str)
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

const char		*get_specs_2(const char *fstr, t_specs *specs)
{
	specs->type = *fstr;
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
	return (fstr);
}

const char		*get_specs_1(va_list ap, const char *fstr, t_specs *s)
{
	const char	fields_list[] = "-+ 0#.lh*123456789";
	const char	*flag;

	while (*fstr && (flag = ft_memchr(fields_list, *fstr, sizeof(fields_list))))
	{
		if (flag - fields_list < 5)
		{
			s->flags |= (1 << (flag - fields_list));
			fstr++;
			continue;
		}
		if (*fstr == '*' || ft_isdigit(*fstr))
			s->width = (*fstr++ == '*' ? va_arg(ap, int) : atoi_shift(&fstr));
		if (*fstr == '.')
		{
			fstr++;
			s->precision = *fstr++ == '*' ? va_arg(ap, int) : atoi_shift(&fstr);
			s->precision >= 0 ? s->flags |= FLAG_PRECISION : 0;
		}
		if (*fstr == 'h')
			*++fstr == 'h' ? (s->len |= LEN_HH) && fstr++ : (s->len |= LEN_H);
		else if (*fstr == 'l')
			*++fstr == 'l' ? (s->len |= LEN_LL) && fstr++ : (s->len |= LEN_L);
	}
	return (get_specs_2(fstr, s));
}

static int		reformat(char *res, va_list ap, const char **fstr, int count)
{
	t_specs	specs;

	specs = (t_specs){0, 0, 0, 0, '\0'};
	*fstr = get_specs_1(ap, *fstr, &specs);
	if (**fstr)
		(*fstr)++;

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

	if (specs.type == 's' && specs.len != LEN_L)
		count += reformat_string(res, va_arg(ap, char *), specs);
	else if (specs.type == 's' && specs.len == LEN_L)
		count += reformat_string_utf8(res, va_arg(ap, wchar_t *), specs);
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
