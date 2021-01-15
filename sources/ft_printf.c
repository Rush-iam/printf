/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/03 21:35:04 by ngragas           #+#    #+#             */
/*   Updated: 2021/01/16 00:06:51 by ngragas          ###   ########.fr       */
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

int			infin_mult(t_float *dst, int m1_len, __uint128_t m2)
{
	char	temp[sizeof(dst->str)];
	int		m2_shift;
	int		m1_i;
	int		idx;

	ft_memcpy(temp + sizeof(dst->str) - m1_len,
								dst->str + sizeof(dst->str) - m1_len, m1_len);
	ft_memset(dst->str, 0, sizeof(dst->str));
	m2_shift = 0;
	while (m2)
	{
		m1_i = 0;
		while (m1_i < m1_len)
		{
			idx = sizeof(dst->str) - ++m1_i;
			dst->str[idx - m2_shift] += temp[idx] * (m2 % 10);
			if (dst->str[idx - m2_shift] >= 10)
			{
				dst->str[idx - m2_shift - 1] += dst->str[idx - m2_shift] / 10;
				dst->str[idx - m2_shift] %= 10;
			}
		}
		++m2_shift && (m2 /= 10);
	}
	return (m1_len + m2_shift - 1 + (dst->str[idx - m2_shift] != 0));
}

int			infin_div(t_float *dst, int num_len, __uint128_t div)
{
	unsigned long long	mult;
	int					cur_div;

	dst->dot = div;
	while (div > 19)
	{
		div -= 19;
		num_len = infin_mult(dst, num_len,
								10000000000000000000ULL * (1. / (1 << 19)));
	}
	cur_div = div;
	mult = 10;
	while (--div)
		mult *= 10;
	num_len = infin_mult(dst, num_len, mult * (1. / (1 << cur_div)));
	return (num_len);
}

int	ft_printf_float_round(t_float *dst, int num_l, t_specs *s)
{
	const int	prec_start = (s->type == 'f') ? dst->dot : num_l - 1;
	const int	prec_end = sizeof(dst->str) - prec_start + s->prec;
	int			i;
	int			n;

	i = (prec_end < (int)sizeof(dst->str)) ? prec_end : sizeof(dst->str) - 1;
	n = sizeof(dst->str);
	while (i < (int)sizeof(dst->str) && dst->str[i] >= 5)
		i++;
	while (--i >= prec_end)
		if (dst->str[i] > 5 || (dst->str[i] == 5 &&
						(dst->str[i - 1] % 2 || i + 1 < (int)sizeof(dst->str))))
		{
			n = i - 1;
			dst->str[n]++;
			while (dst->str[n] > 9)
				!(dst->str[n--] = 0) && dst->str[n]++;
		}
	if (s->type == 'g' && (s->flags & FLAG_HASH) == 0)
		while (dst->prec_no0 > 0 && dst->str[i--] == 0)
			dst->prec_no0--;
	i = 0;
	while (i < (int)sizeof(dst->str))
		dst->str[i++] += '0';
	return (num_l >= (int)sizeof(dst->str) - n ? num_l : sizeof(dst->str) - n);
}

int	ft_printf_ftoa_2(t_float *dst, int exp_bin, long long mantissa, t_specs *s)
{
	int	num_len;

	num_len = 0;
	while (mantissa)
	{
		dst->str[sizeof(dst->str) - 1 - num_len++] = mantissa % 10;
		mantissa /= 10;
	}
	while (exp_bin > 127)
	{
		num_len = infin_mult(dst, num_len, (__uint128_t)1 << 127);
		exp_bin -= 127;
	}
	if (exp_bin >= 0)
		num_len = infin_mult(dst, num_len, (__uint128_t)1 << exp_bin);
	else if (exp_bin < 0)
		num_len = infin_div(dst, num_len, -exp_bin);
	dst->prec_no0 = s->prec;
	return (ft_printf_float_round(dst, num_len, s));
}

int		ft_printf_ftoa(t_float *dst, double num, t_specs *s)
{
	const long long	num_l = *(long long *)&num;
	int				exp_bin;
	long long		mantissa;

	dst->dot = 0;
	if (num_l < 0)
		dst->sign = '-';
	else if (s->flags & FLAG_PLUS)
		dst->sign = '+';
	else if (s->flags & FLAG_SPACE)
		dst->sign = ' ';
	else
		dst->sign = '\0';
	if (num == 0 && (dst->str[sizeof(dst->str) - 1] = '0'))
		return (1);
	exp_bin = num_l >> 52 & 0b11111111111;
	mantissa = num_l & 0b1111111111111111111111111111111111111111111111111111;
	mantissa = (exp_bin != 0) ? mantissa | (1LL << 52) : mantissa << 1;
	while (mantissa && (mantissa & 1) == 0)
	{
		++exp_bin;
		mantissa >>= 1;
	}
	return (ft_printf_ftoa_2(dst, exp_bin - 1023 - 52, mantissa, s));
}

void		ft_printf_float_norm(t_buf *res, t_float *f, t_specs *s)
{
	if (f->int_l)
		ft_printf_bufcpy(res, f->str + sizeof(f->str) - f->src_l, f->int_l);
	ft_printf_bufset(res, '0', f->int_l == 0);
	(s->type == 'g') ? s->prec = f->prec_no0 + f->dot - f->src_l + 1 : 0;
	((s->type == 'f' && s->prec) || (s->type == 'g' && f->prec_no0 >= f->int_l))
				|| s->flags & FLAG_HASH ? ft_printf_bufset(res, '.', 1) : 0;
	if (f->dot > (int)sizeof(f->str))
		(s->prec < f->dot - (int)sizeof(f->str)) ?
			ft_printf_bufset(res, '0', s->prec) :
			ft_printf_bufset(res, '0', f->dot - sizeof(f->str));
	if (f->dot >= (int)sizeof(f->str) && s->prec < f->dot
								&& s->prec > f->dot - (int)sizeof(f->str))
		ft_printf_bufcpy(res, f->str, s->prec - (f->dot - sizeof(f->str)));
	else if (f->dot < (int)sizeof(f->str) && s->prec < f->dot)
		ft_printf_bufcpy(res, f->str + sizeof(f->str) - f->dot, s->prec);
	else if (f->dot < (int)sizeof(f->str))
		ft_printf_bufcpy(res, f->str + sizeof(f->str) - f->dot, f->dot);
	else if (s->prec >= f->dot)
		ft_printf_bufcpy(res, f->str, sizeof(f->str));
	if (s->prec > f->dot && (s->type != 'g' || s->flags & FLAG_HASH))
		ft_printf_bufset(res, '0', s->prec - f->dot);
}

void		ft_printf_float_exp(t_buf *res, t_float *f, t_specs *s)
{
	char	exp_str[3];
	int		i;

	ft_printf_bufcpy(res, f->str + sizeof(f->str) - f->src_l--, 1);
	ft_printf_bufset(res, '.', s->prec || s->flags & FLAG_HASH);
	if (s->prec < f->src_l)
		ft_printf_bufcpy(res, f->str + sizeof(f->str) - f->src_l, s->prec);
	else
	{
		ft_printf_bufcpy(res, f->str + sizeof(f->str) - f->src_l, f->src_l);
		if (s->type != 'g' || s->flags & FLAG_HASH)
			ft_printf_bufset(res, '0', s->prec - f->src_l);
	}
	ft_printf_bufset(res, 'e', 1);
	ft_printf_bufset(res, (f->exp >= 0 ? '+' : '-'), 1);
	if (f->exp < 0)
		f->exp *= -1;
	i = 3;
	while (i > 1 || f->exp)
	{
		exp_str[--i] = '0' + f->exp % 10;
		f->exp /= 10;
	}
	ft_printf_bufcpy(res, exp_str + i, 3 - i);
}

int		ft_printf_float_exceptions(t_buf *res, double num, t_specs *specs)
{
	const int	sign = (*(long long *)&num < 0);
	int			str_len;
	int			width_len;

	str_len = 3 + (num == num &&
							(sign || specs->flags & (FLAG_PLUS | FLAG_SPACE)));
	width_len = (specs->width > str_len) ? specs->width - str_len : 0;
	if ((specs->flags & FLAG_MINUS) == 0)
		ft_printf_bufset(res, ' ', width_len);
	if (num != num)
		ft_printf_bufcpy(res, "nan", 3);
	else
	{
		if (sign)
			ft_printf_bufset(res, '-', 1);
		else if (specs->flags & FLAG_PLUS)
			ft_printf_bufset(res, '+', 1);
		else if (specs->flags & FLAG_SPACE)
			ft_printf_bufset(res, ' ', 1);
		ft_printf_bufcpy(res, "inf", 3);
	}
	if (specs->flags & FLAG_MINUS)
		ft_printf_bufset(res, ' ', width_len);
	return (width_len + str_len);
}

int		ft_printf_float(t_buf *res, double num, t_specs *s)
{
	t_float	f_str;
	char	real_type;

	if (num != num || num == INFINITY || num == -INFINITY)
		return (ft_printf_float_exceptions(res, num, s));
	f_str.src_l = ft_printf_ftoa(&f_str, num, s);
	f_str.int_l = (f_str.src_l > f_str.dot) ? f_str.src_l - f_str.dot : 0;
	f_str.exp = f_str.src_l - f_str.dot - 1;
	if ((real_type = s->type) == 'g')
		real_type = (s->prec >= f_str.exp && f_str.exp >= -4) ? 'f' : 'e';
	s->type == 'g' && real_type == 'e' ? s->prec = f_str.prec_no0 : 0;
	f_str.numout_l = !!f_str.sign + s->prec + ((real_type == 'e') ?
		1 + (s->prec || s->flags & FLAG_HASH) + 4 +
							(((f_str.src_l - f_str.dot - 1) / 100) != 0) :
		f_str.int_l + !f_str.int_l + (s->prec || s->flags & FLAG_HASH));
//	if (specs->type == 'g' && real_type == 'f')
//		f_str.numout_l -= f_str.int_l - (f_str.int_l != 0);
	f_str.width_l = (s->width > f_str.numout_l) ? s->width - f_str.numout_l : 0;
	if ((s->flags & (FLAG_MINUS | FLAG_ZERO)) == 0)
		ft_printf_bufset(res, ' ', f_str.width_l);
	ft_printf_bufset(res, f_str.sign, f_str.sign != '\0');
	(s->flags & FLAG_ZERO) ? ft_printf_bufset(res, '0', f_str.width_l) : 0;
	(real_type == 'f') ? ft_printf_float_norm(res, &f_str, s) :
						ft_printf_float_exp(res, &f_str, s);
	(s->flags & FLAG_MINUS) ? ft_printf_bufset(res, ' ', f_str.width_l) : 0;
	return (f_str.width_l + f_str.numout_l);
}

int		ft_printf_format(t_buf *res, va_list ap, const char **format)
{
	t_specs	specs;
	int		count;

	specs = (t_specs){0, 0, 0, 0, '\0'};
	*format = ft_printf_parse_specs_1(ap, *format, &specs);
	if (specs.type)
		(*format)++;
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
