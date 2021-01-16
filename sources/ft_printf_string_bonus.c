/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_string_bonus.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/16 23:46:23 by ngragas           #+#    #+#             */
/*   Updated: 2021/01/16 23:46:23 by ngragas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf_bonus.h"

int		ft_printf_char(t_buf *res, va_list ap, const t_specs *specs)
{
	char	char_arr[4];
	int		dst_len;
	int		width_len;

	dst_len = 1;
	if (specs->type == 'c' && specs->len == l)
		dst_len = ft_wchrto8(char_arr, va_arg(ap, wint_t));
	else
		*char_arr = (specs->type == 'c') ? (char)va_arg(ap, int) : specs->type;
	if (!dst_len)
		return (-1);
	width_len = (specs->width > dst_len ? specs->width - dst_len : 0);
	if (specs->flags & flag_minus)
		ft_printf_bufcpy(res, char_arr, dst_len);
	if (specs->flags & flag_zero)
		ft_printf_bufset(res, '0', width_len);
	else
		ft_printf_bufset(res, ' ', width_len);
	if ((specs->flags & flag_minus) == 0)
		ft_printf_bufcpy(res, char_arr, dst_len);
	return (width_len + dst_len);
}

int		ft_printf_string(t_buf *res, const char *string, const t_specs *specs)
{
	size_t	src_len;
	size_t	dst_len;
	int		width_len;

	if (string == NULL)
		string = "(null)";
	src_len = ft_strlen(string);
	if ((specs->flags & flag_precision) && (size_t)specs->prec < src_len)
		dst_len = specs->prec;
	else
		dst_len = src_len;
	width_len = ((size_t)specs->width > dst_len ? specs->width - dst_len : 0);
	if (specs->flags & flag_minus)
		ft_printf_bufcpy(res, string, dst_len);
	if (specs->flags & flag_zero)
		ft_printf_bufset(res, '0', width_len);
	else
		ft_printf_bufset(res, ' ', width_len);
	if ((specs->flags & flag_minus) == 0)
		ft_printf_bufcpy(res, string, dst_len);
	return (width_len + dst_len);
}

int		ft_printf_string_utf(t_buf *res, const wchar_t *string,
							const t_specs *specs)
{
	size_t	dst_len;
	int		width_len;

	if (string == NULL)
		string = L"(null)";
	if (specs->flags & flag_precision)
		dst_len = ft_printf_string_utfcpy(NULL, string, specs->prec);
	else
		dst_len = ft_printf_string_utfcpy(NULL, string, -1);
	if (dst_len == (size_t)-1)
		return (-1);
	width_len = ((size_t)specs->width > dst_len ? specs->width - dst_len : 0);
	if (specs->flags & flag_minus)
		ft_printf_string_utfcpy(res, string, dst_len);
	if (specs->flags & flag_zero)
		ft_printf_bufset(res, '0', width_len);
	else
		ft_printf_bufset(res, ' ', width_len);
	if ((specs->flags & flag_minus) == 0)
		ft_printf_string_utfcpy(res, string, dst_len);
	return (width_len + dst_len);
}

/*
**	ft_printf_string_utfcpy
** if res == NULL: return length in bytes without writing
** if n == -1: SIZE_T_MAX limit (==unlimited)
** if UTF-32 >= 1114112 (0x10FFFF): return error (-1)
*/

size_t	ft_printf_string_utfcpy(t_buf *res, const wchar_t *src_utf32, size_t n)
{
	char	char_arr[4];
	int		char_size;
	size_t	count;

	count = 0;
	while (*src_utf32 && count < n)
	{
		if (*src_utf32 < 128)
			char_size = 1;
		else if (*src_utf32 < 2048)
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
		if (res && ft_wchrto8(char_arr, *src_utf32))
			ft_printf_bufcpy(res, char_arr, char_size);
		src_utf32++;
	}
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
