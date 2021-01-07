/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_int.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/06 20:02:42 by ngragas           #+#    #+#             */
/*   Updated: 2021/01/07 17:22:36 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

long long	ft_printf_int_get(va_list ap, const t_specs *specs)
{
	if (specs->type == 'p')
		return ((unsigned long)va_arg(ap, void *));
	else if (specs->type == 'd' || specs->type == 'i')
	{
		if (specs->len == LEN_LL)
			return (va_arg(ap, long long int));
		else if (specs->len == LEN_L)
			return (va_arg(ap, long int));
		else if (specs->len == LEN_H)
			return ((short int)va_arg(ap, int));
		else if (specs->len == LEN_HH)
			return ((char)va_arg(ap, int));
		else
			return (va_arg(ap, int));
	}
	if (specs->len == LEN_LL)
		return (va_arg(ap, unsigned long long int));
	else if (specs->len == LEN_L)
		return (va_arg(ap, unsigned long int));
	else if (specs->len == LEN_H)
		return ((unsigned short int)va_arg(ap, unsigned int));
	else if (specs->len == LEN_HH)
		return ((unsigned char)va_arg(ap, unsigned int));
	else
		return (va_arg(ap, unsigned int));
}

int			ft_printf_int(t_buf *res, long long num, int base, const t_specs *s)
{
	char	num_str[23];
	int		src_l;
	int		prec_l;
	int		width_l;

	if (s->type == 'd' || s->type == 'i')
		src_l = ft_printf_itoa_signed(num_str, num, s);
	else
		src_l = ft_printf_itoa_unsigned(num_str, num, base, s);
	prec_l = (s->precision > src_l) ? s->precision - src_l : 0;
	width_l = (s->width > (num_str[0] != 0) + prec_l + src_l) ?
		s->width - (num_str[0] != 0) - (num_str[1] != 0) - prec_l - src_l : 0;
	if ((s->flags & (FLAG_MINUS | FLAG_ZERO)) == 0)
		ft_printf_bufset(res, ' ', width_l);
	if (num_str[0] != '\0')
		ft_printf_bufset(res, num_str[0], 1);
	if (num_str[1] != '\0')
		ft_printf_bufset(res, num_str[1], 1);
	if (s->flags & FLAG_ZERO)
		ft_printf_bufset(res, '0', width_l);
	ft_printf_bufset(res, '0', prec_l);
	ft_printf_bufcpy(res, num_str + (23 - src_l), src_l);
	if (s->flags & FLAG_MINUS)
		ft_printf_bufset(res, ' ', width_l);
	return (width_l + (num_str[0] != 0) + (num_str[1] != 0) + prec_l + src_l);
}

int			ft_printf_itoa_signed(char *dst, long long num,
									const t_specs *specs)
{
	const char	sign = num < 0 ? -1 : 1;
	int			count;

	count = 0;
	while (num)
	{
		dst[22 - count] = '0' + num % 10 * sign;
		num /= 10;
		count++;
	}
	if (sign >= 0)
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
	dst[1] = '\0';
	return (count);
}

int			ft_printf_itoa_unsigned(char *dst, unsigned long long num, int base,
									const t_specs *specs)
{
	int			count;
	int			prefix_0x;
	const char	digits[] = "0123456789abcdef";
	const char	digits_upper[] = "0123456789ABCDEF";

	prefix_0x = specs->type == 'p' ||
				(base == 16 && (specs->flags & FLAG_HASH) && num);
	count = 0;
	while (num)
	{
		if (specs->type == 'X')
			dst[22 - count] = digits_upper[num % base];
		else
			dst[22 - count] = digits[num % base];
		num /= base;
		count++;
	}
	if (prefix_0x)
	{
		dst[0] = '0';
		dst[1] = (specs->type == 'X') ? 'X' : 'x';
	}
	else
		dst[0] = '\0';
	return (count);
}
