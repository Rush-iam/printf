/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/03 21:34:53 by ngragas           #+#    #+#             */
/*   Updated: 2020/12/30 19:20:18 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PRINTF_H
# define FT_PRINTF_H

# include "libft.h"
# include <unistd.h>
# include <stdarg.h>

int					ft_printf(const char *format, ...);

/*
** length: 'hh' = -2; 'h' = -1; 'l' = 1; 'll' = 2
**	flags:	'-'	=1		'+'	=2		' '	=4		'0'	=8		'#'	=16		'.' = 32
**				=000001		=000010		=000100		=001000		=010000	 =100000
*/

# define FLAG_MINUS		1
# define FLAG_PLUS		2
# define FLAG_SPACE		4
# define FLAG_ZERO		8
# define FLAG_HASH		16
# define FLAG_PRECISION	32

typedef struct		s_specs
{
	unsigned char	flags;
	int				width;
	int				precision;
	char			length;
	char			type;
	const char		*format_end;
}					t_specs;

#endif
