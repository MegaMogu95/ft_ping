NAME    = ft_ping

CC      = cc
CFLAGS  = -Wall -Wextra -Werror
INCLUDES= -I includes/
LDFLAGS = -lm

SRCS    =	src/main.c		\
            src/options.c	\
            src/ping.c		\
            src/icmp.c		\
            src/utils.c

OBJS    := $(SRCS:src/%.c=obj/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS) $(LDFLAGS)
	sudo chmod u+s $(NAME)

obj/%.o: src/%.c | obj
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

obj:
	mkdir -p obj

clean:
	rm -rf obj

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re