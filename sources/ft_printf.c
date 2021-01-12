/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/03 21:35:04 by ngragas           #+#    #+#             */
/*   Updated: 2021/01/12 22:08:12 by ngragas          ###   ########.fr       */
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

int			infin_mult(t_float *dst, int max_shift, __uint128_t mult)
{
	char	temp[sizeof(dst->s)];
	int		shift;
	int		i;
	int		cur_i;

	ft_memcpy(temp + sizeof(dst->s) - max_shift,
			dst->s + sizeof(dst->s) - max_shift, max_shift);
	ft_memset(dst->s, 0, sizeof(dst->s));
	shift = 0;
	while (mult)
	{
		i = 0;
		while (i < max_shift)
		{
			cur_i = sizeof(dst->s) - ++i;
			dst->s[cur_i - shift] += temp[cur_i] * (mult % 10);
			if (dst->s[cur_i - shift] >= 10)
			{
				dst->s[cur_i - shift - 1] += dst->s[cur_i - shift] / 10;
				dst->s[cur_i - shift] %= 10;
			}
		}
		shift++;
		mult /= 10;
	}
	return (shift + i - 1 + (dst->s[sizeof(dst->s) - shift - i] != 0));
}

int			infin_div(t_float *dst, int max_shift, __uint128_t div)
{
	unsigned long long	mult;
	int					cur_div;

	dst->point_pos = div;
	while (div > 19)
	{
		div -= 19;
		max_shift = infin_mult(dst, max_shift,
								10000000000000000000ULL * (1. / (1 << 19)));
	}
	cur_div = div;
	mult = 10;
	while (--div)
		mult *= 10;
	max_shift = infin_mult(dst, max_shift, mult * (1. / (1 << cur_div)));
	return (max_shift);
}

int	ft_printf_ftoa_2(t_float *dst, int exponent, long long mantissa)
{
	int	max_shift;
	int	i;

	max_shift = 0;
	while (mantissa)
	{
		dst->s[sizeof(dst->s) - 1 - max_shift++] = mantissa % 10;
		mantissa /= 10;
	}
	while(exponent > 127)
	{
		max_shift = infin_mult(dst, max_shift, (__uint128_t)1 << 127);
		exponent -= 127;
	}
	if (exponent >= 0)
		max_shift = infin_mult(dst, max_shift, (__uint128_t)1 << exponent);
	else if (exponent < 0)
		max_shift = infin_div(dst, max_shift, -exponent);
	i = sizeof(dst->s) - max_shift;
	while (i < (int)sizeof(dst->s))
		dst->s[i++] += '0';
	return (max_shift);
}

int	ft_printf_ftoa(t_float *dst, double num, const t_specs *specs)
{
	const long long	num_l = *(long long *)&num;
	int				exponent;
	long long		mantissa;

	dst->sign = '\0';
	if (num != num && ft_memcpy(dst->s + sizeof(dst->s) - 3, "nan", 3))
		return (3);
	if (num_l < 0)
		dst->sign = '-';
	else if (specs->flags & FLAG_PLUS)
		dst->sign = '+';
	else if (specs->flags & FLAG_SPACE)
		dst->sign = ' ';
	if (num == 0 && (dst->s[sizeof(dst->s) - 1] = '0'))
		return (1);
	if ((num == INFINITY || num == -INFINITY)
						&& ft_memcpy(dst->s + sizeof(dst->s) - 3, "inf", 3))
		return (3);
	exponent = num_l >> 52 & 0b11111111111;
	mantissa = num_l & 0b1111111111111111111111111111111111111111111111111111;
	mantissa = (exponent != 0) ? mantissa | (1LL << 52) : mantissa << 1;
	while (mantissa && (mantissa & 1) == 0)
		++exponent && (mantissa >>= 1);
	printf("REAL exp = %d; mantissa = %lld\n", exponent - 1023 - 52, mantissa);//
	return (ft_printf_ftoa_2(dst, exponent - 1023 - 52, mantissa));
}

int			ft_printf_float(t_buf *res, double num, const t_specs *specs)
{
	t_float	float_str;
	int		src_l;
	int		prec_l;
	int		width_l;

	float_str.point_pos = 0;
	src_l = ft_printf_ftoa(&float_str, num, specs);
	ft_printf_bufcpy(res, &float_str.sign, (float_str.sign != '\0'));
	ft_printf_bufcpy(res, float_str.s + (sizeof(float_str.s) - src_l), src_l);
	printf("\nsrc_l=%d; point=%d; e=%d\n", src_l, float_str.point_pos,
		   src_l - float_str.point_pos - 1); //
	return (0);
//	prec_l = 0;
//	if (specs->precision > src_l)
//		prec_l = specs->precision - src_l;
//	width_l = 0;
//	if (specs->width > (float_str[0] != '\0') + prec_l + src_l)
//		width_l = specs->width - (float_str[0] != '\0') - prec_l - src_l;
//	if ((specs->flags & (FLAG_MINUS | FLAG_ZERO)) == 0)
//		ft_printf_bufset(res, ' ', width_l);
//	ft_printf_bufcpy(res, float_str, (float_str[0] != '\0'));
//	if (specs->flags & FLAG_ZERO)
//		ft_printf_bufset(res, '0', width_l);
//	ft_printf_bufset(res, '0', prec_l);
//	ft_printf_bufcpy(res, float_str + (23 - src_l), src_l);
//	if (specs->flags & FLAG_MINUS)
//		ft_printf_bufset(res, ' ', width_l);
//	return (width_l + (float_str[0] != '\0') + prec_l + src_l);
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
