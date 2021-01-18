/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_int.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/06 20:02:42 by ngragas           #+#    #+#             */
/*   Updated: 2021/01/18 22:29:08 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

long long	ft_printf_int_get(va_list ap, const t_specs *specs)
{
	if (specs->type == 'p')
		return ((unsigned long)va_arg(ap, void *));
	else if (specs->type == 'd' || specs->type == 'i')
	{
		if (specs->len == 0)
			return (va_arg(ap, int));
		else if (specs->len == ll)
			return (va_arg(ap, long long int));
		else if (specs->len == l)
			return (va_arg(ap, long int));
		else if (specs->len == hh)
			return ((char)va_arg(ap, int));
		else if (specs->len == h)
			return ((short int)va_arg(ap, int));
	}
	if (specs->len == ll)
		return (va_arg(ap, unsigned long long int));
	else if (specs->len == l)
		return (va_arg(ap, unsigned long int));
	else if (specs->len == hh)
		return ((unsigned char)va_arg(ap, unsigned int));
	else if (specs->len == h)
		return ((unsigned short int)va_arg(ap, unsigned int));
	else
		return (va_arg(ap, unsigned int));
}

int			ft_printf_int(t_buf *res, long long num, int base,
							const t_specs *specs)
{
	char	num_str[23];
	int		src_l;
	int		prec_l;
	int		width_l;

	if (specs->type == 'd' || specs->type == 'i')
		src_l = ft_printf_itoa_signed(num_str, num, specs);
	else
		src_l = ft_printf_itoa_unsigned(num_str, num, base, specs);
	prec_l = (specs->prec > src_l) ? specs->prec - src_l : 0;
	if (specs->type == 'o' && prec_l != 0 && specs->flags & flag_hash)
		num_str[0] = '\0';
	width_l = 0;
	if (specs->width > (num_str[0] != 0) + (num_str[1] != 0) + prec_l + src_l)
		width_l = specs->width - !!num_str[0] - !!num_str[1] - prec_l - src_l;
	if ((specs->flags & (flag_minus | flag_zero)) == 0)
		ft_printf_bufset(res, ' ', width_l);
	ft_printf_bufcpy(res, num_str, (num_str[0] != 0) + (num_str[1] != 0));
	if (specs->flags & flag_zero)
		ft_printf_bufset(res, '0', width_l);
	ft_printf_bufset(res, '0', prec_l);
	ft_printf_bufcpy(res, num_str + (23 - src_l), src_l);
	if (specs->flags & flag_minus)
		ft_printf_bufset(res, ' ', width_l);
	return (width_l + (num_str[0] != 0) + (num_str[1] != 0) + prec_l + src_l);
}

/*
**	ft_printf_itoa
**	signed: num[0] holds sign '-'/'+'/' '
**  unsigned: num[0]:num[1] hold prefix "0x"/"0X"
*/

int			ft_printf_itoa_signed(char *dst, long long num,
									const t_specs *specs)
{
	const char	sign = (num < 0) ? -1 : 1;
	int			count;

	count = 0;
	while (num)
	{
		dst[22 - count] = '0' + num % 10 * sign;
		num /= 10;
		count++;
	}
	if (sign == 1)
	{
		if (specs->flags & flag_plus)
			dst[0] = '+';
		else if (specs->flags & flag_space)
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
	const char	digits_upper[] = "0123456789ABCDEF";
	const char	digits[] = "0123456789abcdef";
	int			prefix_0x;
	int			count;

	prefix_0x = (specs->type == 'p') ||
				(base == 16 && (specs->flags & flag_hash) && num);
	count = 0;
	if (prefix_0x || (base == 8 && specs->flags & flag_hash))
		dst[0] = '0';
	else
		dst[0] = '\0';
	if (prefix_0x)
		dst[1] = (specs->type == 'X') ? 'X' : 'x';
	else
		dst[1] = '\0';
	while (num)
	{
		dst[22 - count++] = (specs->type == 'X') ?
							digits_upper[num % base] : digits[num % base];
		num /= base;
	}
	return (count);
}
