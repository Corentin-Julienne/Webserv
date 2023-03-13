# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mpeharpr <mpeharpr@student.s19.be>         +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/02/16 12:16:41 by cjulienn          #+#    #+#              #
#    Updated: 2023/03/13 17:10:35 by mpeharpr         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# colors set
RED			= \033[1;31m
GREEN		= \033[1;32m
YELLOW		= \033[1;33m
BLUE		= \033[1;34m
MAGENTA		= \033[1;35m
CYAN		= \033[1;36m
WHITE		= \033[1;37m
UNDERLINE	= \e[4m
RESET		= \033[0m
END			= \e[0m

NAME := webserv

CC := c++
CFLAGS := -Wall -Wextra -Werror -std=c++98 -g

SRC_DIR := srcs
OBJ_DIR := objs
SUB_DIRS := server parser # update this when adding new subdir
SOURCEDIRS := $(foreach dir, $(SUB_DIRS), $(addprefix $(SRC_DIR)/, $(dir)))

SRC_FILES := $(foreach dir,$(SOURCEDIRS),$(wildcard $(dir)/*.cpp))
OBJ_FILES := $(addprefix $(OBJ_DIR)/,$(notdir $(SRC_FILES:.cpp=.o)))

RM = rm -rf
MKDIR = mkdir -p

VPATH = $(SOURCEDIRS)

all: $(NAME)

$(NAME): $(OBJ_FILES)
	@printf "$(YELLOW)Linking Webserv...\n\n$(END)"
	$(CC) $(CFLAGS) $(OBJ_FILES) -o $(NAME)
	@printf "\n$(GREEN)Webserv compiled.\n$(END)$(GREEN)Simply type$(END) $(WHITE)./webserv $(END)"
	@printf "$(GREEN)with a configuration file as argument to execute the program. \n\n$(END)"

$(OBJ_DIR)/%.o : %.cpp
	@$(MKDIR) $(OBJ_DIR)
	@printf "$(YELLOW)Compiling object files :\n$(END)"
	@$(CC) $(CFLAGS) -c -o $@ $<
	@printf "$(GREEN)Object $(UNDERLINE)$(WHITE)$(notdir $@)$(END)$(GREEN) successfully compiled\n\n$(END)"

clean:
	@printf "$(YELLOW)Removing object files...\n$(END)"
	$(RM) $(OBJ_DIR)
	@printf "$(GREEN)Object files removed!\n\n$(END)"

fclean: clean
	@printf "$(YELLOW)Removing object files and Webserv executable...\n$(END)"
	$(RM) $(NAME)
	@printf "$(GREEN)All clean!\n\n$(END)"

re: fclean all

.PHONY: all clean fclean re
