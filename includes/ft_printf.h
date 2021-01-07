/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/03 21:34:53 by ngragas           #+#    #+#             */
/*   Updated: 2021/01/07 18:55:30 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PRINTF_H
# define FT_PRINTF_H

# include "libft.h"
# include <unistd.h>
# include <stdarg.h>
# include <wchar.h>

/*
**	flags:	'-'	=1		'+'	=2		' '	=4		'0'	=8		'#'	=16		'.' = 32
**				=000001		=000010		=000100		=001000		=010000	 =100000
** length: 'hh' =1		'h' =3		'l' =7		'll'=15
**				=0001		=0011		=0111		=1111
*/

# define FLAG_MINUS		0b000001
# define FLAG_PLUS		0b000010
# define FLAG_SPACE		0b000100
# define FLAG_ZERO		0b001000
# define FLAG_HASH		0b010000
# define FLAG_PRECISION	0b100000

# define LEN_HH	0b0001
# define LEN_H	0b0011
# define LEN_L	0b0111
# define LEN_LL	0b1111

typedef struct		s_buf
{
	char			str[4096];
	int				len;
	int				total_count;
}					t_buf;

typedef struct		s_specs
{
	char			flags;
	int				width;
	int				precision;
	char			len;
	char			type;
}					t_specs;

int					ft_printf(const char *format, ...);
int					ft_printf_format(t_buf *res, va_list ap,
														const char **format);
void				ft_printf_put_count(va_list ap, int count, char length);
void				ft_printf_bufcpy(t_buf *buf, const char *src, size_t n);
void				ft_printf_bufset(t_buf *buf, char c, size_t n);

const char			*ft_printf_parse_specs_1(va_list ap, const char *fstr,
																t_specs *s);
const char			*ft_printf_parse_specs_2(const char *fstr, t_specs *specs);
int					ft_printf_parse_atoi(const char **str);

int					ft_printf_char(t_buf *res, va_list ap,
														const t_specs *specs);
int					ft_printf_string(t_buf *res, const char *string,
														const t_specs *specs);
int					ft_printf_string_utf(t_buf *res, const wchar_t *string,
														const t_specs *specs);
size_t				ft_printf_string_utfcpy(t_buf *res,
											const wchar_t *src_utf32, size_t n);
int					ft_wchrto8(char *dst_utf8, wchar_t src_utf32);

int					ft_printf_int(t_buf *res, long long num, int base,
															const t_specs *s);
long long			ft_printf_int_get(va_list ap, const t_specs *specs);
int					ft_printf_itoa_signed(char *dst, long long num,
														const t_specs *specs);
int					ft_printf_itoa_unsigned(char *dst, unsigned long long num,
												int base, const t_specs *specs);

#endif
