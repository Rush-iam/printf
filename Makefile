# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ngragas <ngragas@student.21-school.ru>     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2020/10/04 20:29:14 by ngragas           #+#    #+#              #
#    Updated: 2020/12/28 19:28:49 by ngragas          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = libftprintf.a
CFLAGS = -Wall -Wextra -Werror -MMD -O2
SRC = ft_printf.c
OBJ = $(addprefix $(OBJ_DIR), $(SRC:.c=.o))
DEP = $(OBJ:.o=.d)
LIB = $(LIB_DIR)libft.a
INCL_DIR = includes/
SRC_DIR = sources/
OBJ_DIR = objects/
LIB_DIR = libft/

all: $(NAME)
$(LIB): FORCE
	$(MAKE) -C $(LIB_DIR)
$(NAME): $(OBJ) $(LIB)
	cp $(LIB) $(NAME)
	$(AR) rcs $(NAME) $<
$(OBJ): | $(OBJ_DIR)
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)
$(OBJ_DIR)%.o: $(SRC_DIR)%.c
	$(CC) $(CFLAGS) -c $< -o $@ -I$(INCL_DIR) -I$(LIB_DIR)
-include $(DEP)

clean:
	$(RM) $(OBJ)
	$(RM) $(DEP)
	rmdir $(OBJ_DIR)
	$(MAKE) -C $(LIB_DIR) $@
fclean: clean
	$(RM) $(NAME)
	$(MAKE) -C $(LIB_DIR) $@
# bonus:!!!!!!
re: fclean all
norm:
	@norminette $(SRC_DIR)* $(INCL_DIR)*
.PHONY: FORCE all clean fclean re libft bonus norm
FORCE: