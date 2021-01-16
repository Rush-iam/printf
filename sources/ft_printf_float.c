/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_float.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/16 16:52:50 by ngragas           #+#    #+#             */
/*   Updated: 2021/01/16 19:21:34 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int		ft_printf_float(t_buf *res, double num, t_specs *s)
{
	t_float	f_str;
	char	real_type;

	if (num != num || num == INFINITY || num == -INFINITY)
		return (ft_printf_float_exceptions(res, num, s));
	f_str.src_l = ft_printf_ftoa(&f_str, num, s);
	f_str.int_l = (f_str.src_l > f_str.dot) ? f_str.src_l - f_str.dot : 0;
	f_str.exp = f_str.src_l - f_str.dot - 1;
	real_type = s->type;
	if (real_type == 'g')
		real_type = ft_printf_float_g(&f_str, s);
	f_str.numout_l = !!f_str.sign + s->prec + (s->prec || s->flags & flag_hash)
	+ ((real_type == 'e') ? 5 + (((f_str.src_l - f_str.dot - 1) / 100) != 0) :
							f_str.int_l + (f_str.int_l == 0));
	f_str.width_l = (s->width > f_str.numout_l) ? s->width - f_str.numout_l : 0;
	if ((s->flags & (flag_minus | flag_zero)) == 0)
		ft_printf_bufset(res, ' ', f_str.width_l);
	ft_printf_bufset(res, f_str.sign, f_str.sign != '\0');
	if (s->flags & flag_zero)
		ft_printf_bufset(res, '0', f_str.width_l);
	(real_type == 'f') ? ft_printf_float_norm(res, &f_str, s) :
	ft_printf_float_exp(res, &f_str, s);
	if (s->flags & flag_minus)
		ft_printf_bufset(res, ' ', f_str.width_l);
	return (f_str.width_l + f_str.numout_l);
}

void	ft_printf_float_norm(t_buf *res, t_float *f, t_specs *s)
{
	if (f->int_l)
		ft_printf_bufcpy(res, f->str + sizeof(f->str) - f->src_l, f->int_l);
	ft_printf_bufset(res, '0', f->int_l == 0);
	if (s->flags & flag_hash || s->prec)
		ft_printf_bufset(res, '.', 1);
	if (f->dot > (int)sizeof(f->str))
	{
		if (s->prec < f->dot - (int)sizeof(f->str))
			ft_printf_bufset(res, '0', s->prec);
		else
			ft_printf_bufset(res, '0', f->dot - sizeof(f->str));
	}
	if (f->dot >= (int)sizeof(f->str) && s->prec < f->dot
		&& s->prec > f->dot - (int)sizeof(f->str))
		ft_printf_bufcpy(res, f->str, s->prec - (f->dot - sizeof(f->str)));
	else if (f->dot < (int)sizeof(f->str) && s->prec < f->dot)
		ft_printf_bufcpy(res, f->str + sizeof(f->str) - f->dot, s->prec);
	else if (f->dot < (int)sizeof(f->str))
		ft_printf_bufcpy(res, f->str + sizeof(f->str) - f->dot, f->dot);
	else if (s->prec >= f->dot)
		ft_printf_bufcpy(res, f->str, sizeof(f->str));
	if (s->prec > f->dot && (s->type != 'g' || s->flags & flag_hash))
		ft_printf_bufset(res, '0', s->prec - f->dot);
}

void	ft_printf_float_exp(t_buf *res, t_float *f, t_specs *specs)
{
	char	exp_str[3];
	int		i;

	ft_printf_bufcpy(res, f->str + sizeof(f->str) - f->src_l--, 1);
	ft_printf_bufset(res, '.', specs->prec || specs->flags & flag_hash);
	if (specs->prec < f->src_l)
		ft_printf_bufcpy(res, f->str + sizeof(f->str) - f->src_l, specs->prec);
	else
	{
		ft_printf_bufcpy(res, f->str + sizeof(f->str) - f->src_l, f->src_l);
		if (specs->type != 'g' || specs->flags & flag_hash)
			ft_printf_bufset(res, '0', specs->prec - f->src_l);
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

char	ft_printf_float_g(t_float *f_str, t_specs *specs)
{
	char	real_type;

	if (specs->prec >= f_str->exp && f_str->exp >= -4)
		real_type = 'f';
	else
		real_type = 'e';
	if (real_type == 'e' || !(specs->flags & flag_hash))
		specs->prec = f_str->prec_no0;
	if (real_type == 'f')
		specs->prec += f_str->dot - f_str->src_l + 1;
	if (specs->prec < 0)
		specs->prec = 0;
	if ((specs->flags & flag_hash) == 0)
	{
		if (real_type == 'f' && specs->prec > f_str->dot)
			specs->prec = f_str->dot;
		if (real_type == 'e' && specs->prec >= f_str->src_l)
			specs->prec = f_str->src_l - 1;
	}
	return (real_type);
}

int		ft_printf_float_exceptions(t_buf *res, double num, t_specs *specs)
{
	const int	sign = (*(long long *)&num < 0);
	int			str_len;
	int			width_len;

	str_len = 3 + (num == num &&
							(sign || specs->flags & (flag_plus | flag_space)));
	width_len = (specs->width > str_len) ? specs->width - str_len : 0;
	if ((specs->flags & flag_minus) == 0)
		ft_printf_bufset(res, ' ', width_len);
	if (num != num)
		ft_printf_bufcpy(res, "nan", 3);
	else
	{
		if (sign)
			ft_printf_bufset(res, '-', 1);
		else if (specs->flags & flag_plus)
			ft_printf_bufset(res, '+', 1);
		else if (specs->flags & flag_space)
			ft_printf_bufset(res, ' ', 1);
		ft_printf_bufcpy(res, "inf", 3);
	}
	if (specs->flags & flag_minus)
		ft_printf_bufset(res, ' ', width_len);
	return (width_len + str_len);
}
