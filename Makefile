# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: asabri <asabri@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/02/03 20:20:25 by asabri            #+#    #+#              #
#    Updated: 2024/03/01 22:35:06 by asabri           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = irc
CC              = c++ # -fsanitize=address -g
FLAGS           = -Wall -Wextra -Werror -std=c++98
OBJDIR          = .obj
FILES           = src/main  src/server src/Client src/Command src/Channel src/join src/privmsg src/bot src/topic src/mode src/invite src/kick src/auth src/quit
SRC             = $(FILES:=.cpp)
OBJ             = $(addprefix $(OBJDIR)/, $(FILES:=.o))
HEADER          = inc/server.hpp inc/IrcReplies.hpp inc/Client.hpp inc/Channel.hpp

all: $(NAME)

$(NAME):$(OBJ) $(HEADER)
		$(CC) $(OBJ) -o $(NAME)


$(OBJDIR)/%.o: %.cpp $(HEADER)
		mkdir -p $(dir $@)
		$(CC) $(FLAGS) -c $< -o $@
clean:
		rm -rf $(OBJDIR) $(OBJ)


fclean: clean
		rm -rf  $(NAME)


re: fclean all
.PHONY: all clean fclean re