# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ngragas <ngragas@student.21-school.ru>     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2020/10/04 20:29:14 by ngragas           #+#    #+#              #
#    Updated: 2021/01/16 23:37:35 by ngragas          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = libftprintf.a
CFLAGS = -Wall -Wextra -Werror -MMD -O2
SRC :=	ft_printf.c			ft_printf_parser.c	ft_printf_string.c			\
		ft_printf_int.c		ft_printf_float.c	ft_printf_float_converter.c
SRC_BONUS = $(SRC:.c=_bonus.c)
OBJ = $(addprefix $(OBJ_DIR), $(SRC:.c=.o))
OBJ_BONUS = $(addprefix $(OBJ_DIR), $(SRC_BONUS:.c=.o))
DEP = $(OBJ:.o=.d)
DEP_BONUS = $(OBJ_BONUS:.o=.d)
LIB = $(LIB_DIR)libft.a
INCL_DIR = includes/
SRC_DIR = sources/
OBJ_DIR = objects/
LIB_DIR = libft/

all: $(NAME)
$(LIB): FORCE
	$(MAKE) -C $(LIB_DIR)
$(NAME): $(LIB) $(OBJ)
	cp $(LIB) $(NAME)
	$(AR) rcs $(NAME) $(OBJ)
bonus: $(LIB) $(OBJ_BONUS)
	cp $(LIB) $(NAME)
	$(AR) rcs $(NAME) $(OBJ_BONUS)
$(OBJ): | $(OBJ_DIR)
$(OBJ_BONUS): | $(OBJ_DIR)
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)
$(OBJ_DIR)%.o: $(SRC_DIR)%.c
	$(CC) $(CFLAGS) -c $< -o $@ -I$(INCL_DIR) -I$(LIB_DIR)
-include $(DEP)

clean:
	$(RM) $(OBJ)
	$(RM) $(OBJ_BONUS)
	$(RM) $(DEP)
	$(RM) $(DEP_BONUS)
	rmdir $(OBJ_DIR)
	$(MAKE) -C $(LIB_DIR) $@
fclean: clean
	$(RM) $(NAME)
	$(MAKE) -C $(LIB_DIR) $@
re: fclean all
norm:
	@norminette $(SRC_DIR)* $(INCL_DIR)*
.PHONY: FORCE all clean fclean re libft bonus norm
FORCE: