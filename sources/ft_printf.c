/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/03 21:35:04 by ngragas           #+#    #+#             */
/*   Updated: 2021/01/05 22:18:21 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static int	atoi_shift(const char **str)
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

const char	*get_specs_2(const char *fstr, t_specs *specs)
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

const char	*get_specs_1(va_list ap, const char *fstr, t_specs *s)
{
	const char	fields_list[] = "-+ 0#.lh*123456789";
	const char	*flag;

	while (*fstr && (flag = ft_memchr(fields_list, *fstr, sizeof(fields_list))))
	{
		if (flag - fields_list < 5)
		{
			s->flags = (char)(s->flags | 1 << (flag - fields_list));
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

int		ft_printf_signed_itoa(char *dst, va_list ap, char length, char flags)
{
	long long	src_number;
	char		sign;
	int			count;

	if (length == LEN_LL)
		src_number = va_arg(ap, long long int);
	else if (length == LEN_L)
		src_number = va_arg(ap, long int);
	else
		src_number = va_arg(ap, int);
	sign = (src_number >= 0) ? 1 : -1;
	count = 0;
	while (src_number)
	{
		dst[19 - count++] = (char)('0' + src_number % 10 * sign);
		src_number /= 10;
	}
	dst[0] = '\0';
	if (sign == -1)
		dst[0] = '-';
	else if (flags & FLAG_PLUS)
		dst[0] = '+';
	else if (flags & FLAG_SPACE)
		dst[0] = ' ';
	return (count);
}

int		ft_printf_signed(char *res, va_list ap, t_specs specs)
{
	char	number[20];
	int		src_len;
	int		prec_len;
	int		width_len;

	src_len = ft_printf_signed_itoa(number, ap, specs.len, specs.flags);
	if ((specs.flags & FLAG_PRECISION) && specs.precision > src_len)
		prec_len = specs.precision;
	else
		prec_len = src_len;
	width_len = specs.width > prec_len ? specs.width - prec_len : *number != 0;
	if (specs.flags & FLAG_MINUS)
	{
		number[0] != '\0' ? *res++ = number[0] : 0;
		ft_memset(res, '0', prec_len - src_len) && (res += prec_len - src_len);
		ft_memcpy(res, number + (19 - src_len), src_len) && (res += src_len);
		ft_memset(res, ' ', width_len - (number[0] != '\0'));
	}
	else if (specs.flags & FLAG_ZERO)
	{
		number[0] != '\0' ? *res++ = number[0] : 0;
		ft_memset(res, '0', width_len - (number[0] != '\0'));
		res += width_len - (number[0] != '\0');
		ft_memcpy(res, number + (19 - src_len), src_len);
	}
	else
	{
		ft_memset(res, ' ', width_len - (number[0] != '\0'));
		res += width_len - (number[0] != '\0');
		number[0] != '\0' ? *res++ = number[0] : 0;
		ft_memset(res, '0', prec_len - src_len) && (res += prec_len - src_len);
		ft_memcpy(res, number + (19 - src_len), src_len);
	}
	return (width_len + (number[0] != '\0') + prec_len);
}

static int	reformat(char *res, va_list ap, const char **fstr, int count)
{
	t_specs	specs;
	int		extra_count;

	specs = (t_specs){0, 0, 0, 0, '\0'};
	*fstr = get_specs_1(ap, *fstr, &specs);
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

	extra_count = 0;
	if (specs.type == 's' && specs.len != LEN_L)
		extra_count = ft_printf_string(res, va_arg(ap, char *), specs);
	else if (specs.type == 's' && specs.len == LEN_L)
		extra_count = ft_printf_string_utf(res, va_arg(ap, wchar_t *), specs);
	else if (specs.type == 'd' || specs.type == 'i' || specs.type == 'u')
		extra_count = ft_printf_signed(res, ap, specs);
//	else if (specs.type == 'p' || specs.type == 'x' || specs.type == 'X')
//		count += reformat_hex(res, ap, specs);
//	else if (specs.type == 'f' || specs.type == 'e' || specs.type == 'g')
//		count += reformat_float(res, ap, specs);
//	else if (specs.type == 'n')
//		put_count(va_arg(ap, int *), count, specs);
	else if (specs.type == 'c' || specs.type)
		extra_count = ft_printf_char(res, ap, specs);
	return (extra_count == -1 ? -1 : count + extra_count);
}

int			ft_printf(const char *format, ...)
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
			char_count = reformat(res + char_count, ap, &format, char_count);
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
