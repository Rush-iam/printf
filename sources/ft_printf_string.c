/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_string.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/05 18:48:46 by ngragas           #+#    #+#             */
/*   Updated: 2021/01/05 20:53:00 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int		ft_printf_char(char *res, va_list ap, t_specs specs)
{
	char	character[4];
	int		dst_len;
	int		real_len;

	dst_len = 1;
	if (specs.type == 'c' && specs.len == LEN_L)
		dst_len = ft_wchrto8(character, va_arg(ap, wint_t));
	else
		*character = (char)(specs.type == 'c' ? va_arg(ap, int) : specs.type);
	if (!dst_len)
		return (-1);
	real_len = (specs.width > dst_len ? specs.width : dst_len);
	if (specs.flags & FLAG_MINUS)
	{
		ft_memcpy(res, character, dst_len);
		res += dst_len;
		ft_memset(res, ' ', real_len - dst_len);
	}
	else
	{
		ft_memset(res, " 0"[(specs.flags & FLAG_ZERO) > 0], real_len - dst_len);
		res += real_len - dst_len;
		ft_memcpy(res, character, dst_len);
	}
	return (real_len);
}

int		ft_printf_string(char *res, const char *string, t_specs specs)
{
	const size_t	src_len = ft_strlen(string);
	size_t			dst_len;
	size_t			real_len;

	if ((specs.flags & FLAG_PRECISION) && (size_t)specs.precision < src_len)
		dst_len = specs.precision;
	else
		dst_len = src_len;
	real_len = ((size_t)specs.width > dst_len ? specs.width : dst_len);
	if (specs.flags & FLAG_MINUS)
	{
		ft_memcpy(res, string, dst_len);
		res += dst_len;
		ft_memset(res, ' ', real_len - dst_len);
	}
	else
	{
		ft_memset(res, ' ', real_len - dst_len);
		res += real_len - dst_len;
		ft_memcpy(res, string, dst_len);
	}
	return (real_len);
}

int		ft_printf_string_utf(char *res, const wchar_t *string, t_specs specs)
{
	size_t	dst_len;
	size_t	real_len;

	if (specs.flags & FLAG_PRECISION)
		dst_len = ft_wstrto8(NULL, string, specs.precision);
	else
		dst_len = ft_wstrto8(NULL, string, -1);
	if (dst_len == (size_t)-1)
		return (-1);
	real_len = ((size_t)specs.width > dst_len ? specs.width : dst_len);
	if (specs.flags & FLAG_MINUS)
	{
		ft_wstrto8(res, string, dst_len);
		res += dst_len;
		ft_memset(res, ' ', real_len - dst_len);
	}
	else
	{
		ft_memset(res, ' ', real_len - dst_len);
		res += real_len - dst_len;
		ft_wstrto8(res, string, dst_len);
	}
	return (real_len);
}

/*
**	ft_wstrto8
** if dst == NULL: return length in bytes without writing
** if n == -1: SIZE_T_MAX limit of max len
** if UTF-32 >= 1114112 (0x10FFFF): return error (-1)
*/

size_t	ft_wstrto8(char *dst_utf8, const wchar_t *src_utf32, size_t n)
{
	int		char_size;
	size_t	count;

	count = 0;
	while (*src_utf32 && count < n)
	{
		char_size = 1;
		if (128 <= *src_utf32 && *src_utf32 < 2048)
			char_size = 2;
		else if (*src_utf32 < 65536)
			char_size = 3;
		else if (*src_utf32 < 1114112)
			char_size = 4;
		else if (*src_utf32 >= 1114112)
			return (-1);
		if (count + char_size > n)
			break ;
		count += char_size;
		if (dst_utf8)
			dst_utf8 += ft_wchrto8(dst_utf8, *src_utf32);
		src_utf32++;
	}
	if (dst_utf8)
		*dst_utf8 = '\0';
	return (count);
}

/*
**	ft_wchrto8
** if UTF-32 >= 1114112 (0x10FFFF): return error (0 bytes)
*/

int		ft_wchrto8(char *dst_utf8, wchar_t src_utf32)
{
	int		bytes;
	char	mask;
	int		i;

	bytes = 1;
	if (src_utf32 < 128)
		mask = (char)(0b00000000);
	else if (src_utf32 < 2048 && (bytes += 1))
		mask = (char)(0b11000000);
	else if (src_utf32 < 65536 && (bytes += 2))
		mask = (char)(0b11100000);
	else if (src_utf32 < 1114112 && (bytes += 3))
		mask = (char)(0b11110000);
	else
		return (0);
	dst_utf8[0] = (char)(mask | src_utf32 >> 6 * (bytes - 1));
	i = 0;
	while (++i < bytes)
	{
		dst_utf8[i] = (char)(src_utf32 >> 6 * (bytes - i - 1) & 0b111111);
		dst_utf8[i] |= (char)(0b10000000);
	}
	return (bytes);
}
