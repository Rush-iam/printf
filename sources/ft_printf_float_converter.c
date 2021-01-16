/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_float_converter.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/16 16:51:53 by ngragas           #+#    #+#             */
/*   Updated: 2021/01/16 22:58:54 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	ft_printf_ftoa(t_float *dst, double num, t_specs *s)
{
	const long long	num_l = *(long long *)&num;
	int				exp_bin;
	long long		mantissa;

	dst->dot = 0;
	if (num_l < 0)
		dst->sign = '-';
	else if (s->flags & flag_plus)
		dst->sign = '+';
	else if (s->flags & flag_space)
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

int	ft_printf_ftoa_2(t_float *dst, int exp_bin, long long mantissa, t_specs *s)
{
	int					num_len;
	const __uint128_t	i128 = 1;

	num_len = 0;
	while (mantissa)
	{
		dst->str[sizeof(dst->str) - 1 - num_len++] = mantissa % 10;
		mantissa /= 10;
	}
	while (exp_bin > 127)
	{
		num_len = ft_printf_infin_mult(dst, num_len, i128 << 127);
		exp_bin -= 127;
	}
	if (exp_bin >= 0)
		num_len = ft_printf_infin_mult(dst, num_len, i128 << exp_bin);
	else if (exp_bin < 0)
		num_len = ft_printf_infin_div(dst, num_len, -exp_bin);
	dst->prec_no0 = s->prec;
	return (ft_printf_float_round(dst, num_len, s));
}

int	ft_printf_float_round(t_float *dst, int num_l, t_specs *s)
{
	const int	prec_start = (s->type == 'f') ? dst->dot : num_l - 1;
	const int	prec_end = sizeof(dst->str) - prec_start + s->prec;
	int			i;
	int			n;

	i = (prec_end < (int)sizeof(dst->str)) ? prec_end : sizeof(dst->str) - 1;
	n = sizeof(dst->str);
	while (prec_end >= 0 && i < (int)sizeof(dst->str) && dst->str[i] >= 5)
		i++;
	while (prec_end >= 0 && --i >= prec_end)
		if (dst->str[i] > 5 || (dst->str[i] == 5 &&
						(dst->str[i - 1] % 2 || i + 1 < (int)sizeof(dst->str))))
		{
			n = i - 1;
			dst->str[n]++;
			while (dst->str[n] > 9)
				!(dst->str[n--] = 0) && dst->str[n]++;
		}
	if (s->type == 'g' && num_l != 0 && (s->flags & flag_hash) == 0)
		while (dst->prec_no0 > 0 && dst->str[i--] == 0)
			dst->prec_no0--;
	i = 0;
	while (i < (int)sizeof(dst->str))
		dst->str[i++] += '0';
	return (num_l >= (int)sizeof(dst->str) - n ? num_l : sizeof(dst->str) - n);
}

int	ft_printf_infin_mult(t_float *dst, int m1_len, __uint128_t m2)
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

int	ft_printf_infin_div(t_float *dst, int num_len, __uint128_t div)
{
	unsigned long long	mult;
	int					cur_div;

	dst->dot = div;
	while (div > 19)
	{
		div -= 19;
		num_len = ft_printf_infin_mult(
					dst, num_len, 10000000000000000000ULL * (1. / (1 << 19)));
	}
	cur_div = div;
	mult = 10;
	while (--div)
		mult *= 10;
	num_len = ft_printf_infin_mult(dst, num_len, mult * (1. / (1 << cur_div)));
	return (num_len);
}
