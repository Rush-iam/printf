/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/03 21:35:04 by ngragas           #+#    #+#             */
/*   Updated: 2021/01/08 23:52:27 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int			ft_printf(const char *format, ...)
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

int			infin_mult(char *dst, int exponent, long long mantissa)
{
//	if (exponent == 0)
//		dst[0] += 1;
//	else if (exponent > 0)
//	{
//
//	}
//	else if (exponent < 0)
//	{
//
//	}
}

int			ft_printf_ftoa(char *dst, double num, const t_specs *specs)
{
	const int	exponent = ((*(long long *)&num >> 52) & 0b11111111111) - 1023;
	int			i;
	long long	mantissa;

	if (num >= 0)
	{
		if (specs->flags & FLAG_PLUS)
			dst[0] = '+';
		else if (specs->flags & FLAG_SPACE)
			dst[0] = ' ';
		else
			dst[0] = '\0';
	}
	else
		dst[0] = '-';
	dst++;
	ft_memset(dst, 0, sizeof(dst) - 1);
	i = 1;
	while (i <= 52 && (*(long long *)&num & (1LL << (i - 1))) == 0)
		i++;
	mantissa = (*(long long *)&num &
						0b1111111111111111111111111111111111111111111111111111);
	mantissa |= (1LL << 52); // only if not denormalized
	mantissa >>= (i - 1);
	printf("REAL: exp = %d; mantissa = %lld\n", exponent - (53 - i), mantissa);
	return (infin_mult(dst, exponent - (53 - i), mantissa));
}

int			ft_printf_float(t_buf *res, double num, const t_specs *specs)
{
	char	float_str[666];
	int		src_l;
	int		prec_l;
	int		width_l;

	src_l = ft_printf_ftoa(float_str, num, specs);
	return (0);
	prec_l = 0;
	if (specs->precision > src_l)
		prec_l = specs->precision - src_l;
	width_l = 0;
	if (specs->width > (float_str[0] != '\0') + prec_l + src_l)
		width_l = specs->width - (float_str[0] != '\0') - prec_l - src_l;
	if ((specs->flags & (FLAG_MINUS | FLAG_ZERO)) == 0)
		ft_printf_bufset(res, ' ', width_l);
	ft_printf_bufcpy(res, float_str, (float_str[0] != '\0'));
	if (specs->flags & FLAG_ZERO)
		ft_printf_bufset(res, '0', width_l);
	ft_printf_bufset(res, '0', prec_l);
	ft_printf_bufcpy(res, float_str + (23 - src_l), src_l);
	if (specs->flags & FLAG_MINUS)
		ft_printf_bufset(res, ' ', width_l);
	return (width_l + (float_str[0] != '\0') + prec_l + src_l);
}

int			ft_printf_format(t_buf *res, va_list ap, const char **format)
{
	t_specs	specs;
	int		count;

	specs = (t_specs){0, 0, 0, 0, '\0'};
	*format = ft_printf_parse_specs_1(ap, *format, &specs);
	if (specs.type)
		(*format)++;

//#include <stdio.h> //!!!!!!!!
//	printf("\nspecs.type = %c", specs.type);
//	printf("\nspecs.width = %d", specs.width);
//	printf("\nspecs.precision = %d", specs.precision);
//	printf("\nspecs.length = %hhd", specs.len);
//	printf("\nspecs.flags = \"");
//	if (specs.flags & FLAG_MINUS) printf("-");
//	if (specs.flags & FLAG_PLUS) printf("+");
//	if (specs.flags & FLAG_SPACE) printf(" ");
//	if (specs.flags & FLAG_ZERO) printf("0");
//	if (specs.flags & FLAG_HASH) printf("#");
//	if (specs.flags & FLAG_PRECISION) printf(".");
//	printf("\"\n");

	count = 0;
	if (specs.type == 's' && specs.len != LEN_L)
		count = ft_printf_string(res, va_arg(ap, char *), &specs);
	else if (specs.type == 's' && specs.len == LEN_L)
		count = ft_printf_string_utf(res, va_arg(ap, wchar_t *), &specs);
	else if (specs.type == 'd' || specs.type == 'i' || specs.type == 'u')
		count = ft_printf_int(res, ft_printf_int_get(ap, &specs), 10, &specs);
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

void		ft_printf_put_count(va_list ap, int count, char length)
{
	void	*ptr;

	if (length == 0 && (ptr = va_arg(ap, int *)))
		*(int *)ptr = count;
	else if (length == LEN_LL && (ptr = va_arg(ap, long long int *)))
		*(long long int *)ptr = count;
	else if (length == LEN_L && (ptr = va_arg(ap, long int *)))
		*(long int *)ptr = count;
	else if (length == LEN_HH && (ptr = va_arg(ap, char *)))
		*(char *)ptr = (char)count;
	else if (length == LEN_H && (ptr = va_arg(ap, short int *)))
		*(short int *)ptr = (short int)count;
}

void		ft_printf_bufcpy(t_buf *buf, const char *src, size_t n)
{
	buf->total_count += n;
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

	buf->total_count += n;
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
