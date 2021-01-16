/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_bonus.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/16 23:43:01 by ngragas           #+#    #+#             */
/*   Updated: 2021/01/16 23:43:01 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PRINTF_BONUS_H
# define FT_PRINTF_BONUS_H

# include "libft.h"
# include <unistd.h>
# include <stdarg.h>
# include <wchar.h>
# include <math.h>

/*
**	flags:	'-'	=1		'+'	=2		' '	=4		'0'	=8		'#'	=16		'.' = 32
**				=000001		=000010		=000100		=001000		=010000	 =100000
** length: 'hh' =1		'h' =3		'l' =7		'll'=15
**				=0001		=0011		=0111		=1111
*/

enum	e_flag
{
	flag_minus = 0b000001,
	flag_plus = 0b000010,
	flag_space = 0b000100,
	flag_zero = 0b001000,
	flag_hash = 0b010000,
	flag_precision = 0b100000
};

enum	e_len
{
	hh = 0b0001,
	h = 0b0011,
	l = 0b0111,
	ll = 0b1111
};

typedef struct	s_buf
{
	char		str[4096];
	int			len;
	int			total_count;
}				t_buf;

typedef struct	s_specs
{
	enum e_flag	flags;
	int			width;
	int			prec;
	enum e_len	len;
	char		type;
}				t_specs;

typedef struct	s_float
{
	char		sign;
	char		str[751];
	short int	dot;
	short int	exp;
	short int	src_l;
	short int	int_l;
	short int	prec_no0;
	int			numout_l;
	int			width_l;
}				t_float;

/*
**				ft_printf.c
*/
int				ft_printf(const char *format, ...);
int				ft_printf_format(t_buf *res, va_list ap, const char **format);
void			ft_printf_put_count(va_list ap, int count, char length);
void			ft_printf_bufcpy(t_buf *buf, const char *str, size_t n);
void			ft_printf_bufset(t_buf *buf, char chr, size_t n);

/*
**				ft_printf_parse.c
*/
const char		*ft_printf_parse_specs(va_list ap, const char *frmt,
															t_specs *specs);
const char		*ft_printf_parse_refine(const char *frmt, t_specs *specs);
int				ft_printf_parse_atoi(const char **str);

/*
**				ft_printf_string.c
*/
int				ft_printf_char(t_buf *res, va_list ap, const t_specs *specs);
int				ft_printf_string(t_buf *res, const char *string,
														const t_specs *specs);
int				ft_printf_string_utf(t_buf *res, const wchar_t *string,
														const t_specs *specs);
size_t			ft_printf_string_utfcpy(t_buf *res, const wchar_t *src_utf32,
																	size_t n);
int				ft_wchrto8(char *dst_utf8, wchar_t src_utf32);

/*
**				ft_printf_int.c
*/
int				ft_printf_int(t_buf *res, long long num, int base,
														const t_specs *specs);
long long		ft_printf_int_get(va_list ap, const t_specs *specs);
int				ft_printf_itoa_signed(char *dst, long long num,
														const t_specs *specs);
int				ft_printf_itoa_unsigned(char *dst, unsigned long long num,
												int base, const t_specs *specs);
/*
**				ft_printf_float.c
*/
int				ft_printf_float(t_buf *res, double num, t_specs *s);
void			ft_printf_float_norm(t_buf *res, t_float *f, t_specs *s);
void			ft_printf_float_exp(t_buf *res, t_float *f, t_specs *specs);
char			ft_printf_float_g(t_float *f_str, t_specs *specs);
int				ft_printf_float_exceptions(t_buf *res, double num,
															t_specs *specs);
/*
**				ft_printf_float_converter.c
*/
int				ft_printf_ftoa(t_float *dst, double num, t_specs *s);
int				ft_printf_ftoa_2(t_float *dst, int exp_bin,
												long long mantissa, t_specs *s);
int				ft_printf_float_round(t_float *dst, int num_l, t_specs *s);
int				ft_printf_infin_mult(t_float *dst, int m1_len, __uint128_t m2);
int				ft_printf_infin_div(t_float *dst, int num_len, __uint128_t div);

#endif
