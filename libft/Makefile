# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ngragas <ngragas@student.21-school.ru>     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2020/10/04 20:29:14 by ngragas           #+#    #+#              #
#    Updated: 2021/01/26 18:20:34 by ngragas          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = libft.a
CFLAGS = -Wall -Wextra -Werror -MMD -O2
HEADER = libft.h
SRCS =		ft_isalpha.c	ft_isdigit.c	ft_isalnum.c	ft_isprint.c	\
			ft_isascii.c	ft_toupper.c	ft_tolower.c	ft_strmapi.c	\
			ft_strlen.c		ft_strncmp.c	ft_memcmp.c						\
			ft_memchr.c		ft_strchr.c		ft_strrchr.c	ft_strnstr.c	\
			ft_strlcpy.c	ft_memcpy.c		ft_memccpy.c	ft_memmove.c	\
			ft_calloc.c		ft_bzero.c		ft_memset.c						\
			ft_strdup.c		ft_substr.c		ft_strlcat.c	ft_strjoin.c	\
			ft_strtrim.c	ft_split.c		ft_atoi.c		ft_itoa.c		\
			ft_putchar_fd.c	ft_putstr_fd.c	ft_putendl_fd.c	ft_putnbr_fd.c	\
		ft_lstnew.c			ft_lstadd_front.c		ft_lstadd_back.c		\
		ft_lstdelone.c		ft_lstsize.c			ft_lstlast.c			\
		ft_lstclear.c		ft_lstiter.c			ft_lstmap.c
OBJS = $(SRCS:.c=.o)
DEPS = $(OBJS:.o=.d)

all: $(NAME)
clean:
	$(RM) $(OBJS)
	$(RM) $(DEPS)
fclean: clean
	$(RM) $(NAME)
re: fclean all
norm:
	@norminette $(SRCS) $(HEADER)
-include $(DEPS)
$(NAME): $(OBJS)
	$(AR) rcs $(NAME) $?
.PHONY: all clean fclean re norm
