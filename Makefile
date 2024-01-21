NAME	= ircserv

HPP 	= ./includes/

SRCS	= srcs/*.cpp \

OBJS	= $(SRCS:.cpp=.o)

CC		= c++
FLAGS	= -Wall -Wextra -Werror -std=c++98

GREEN	= \e[0;32m
YELLOW	= \e[0;33m
CYAN	= \e[0;36m
RESET	= \e[0m

all: $(NAME)

$(NAME): $(OBJS)
		@$(CC) $(FLAGS) -I $(HPP) $(OBJS) -o $(NAME)
		@printf "$(GREEN)Executable created$(RESET)\n"

clean:
		@rm -f $(OBJS)
		@printf "$(CYAN)Cleaning objects...$(RESET)\n"

fclean: clean
		@rm -rf $(NAME)
		@printf "$(CYAN)Cleaning all...$(RESET)\n"

re: fclean all

.PHONY: all clean fclean re