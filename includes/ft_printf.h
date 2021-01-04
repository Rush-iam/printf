/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/03 21:34:53 by ngragas           #+#    #+#             */
/*   Updated: 2021/01/04 13:31:31 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PRINTF_H
# define FT_PRINTF_H

# include "libft.h"
# include <unistd.h>
# include <stdarg.h>

int					ft_printf(const char *format, ...);

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

typedef struct		s_specs
{
	unsigned char	flags;
	int				width;
	int				precision;
	char			len;
	char			type;
}					t_specs;

#endif
