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

# ── parsing unit test ───────────────────────────────────────────────
MAND_DIR     = mandatory
MAND_INC     = $(MAND_DIR)/includes
TEST_DIR     = $(MAND_DIR)/test
PARSING_BIN  = $(TEST_DIR)/test_parsing
PARSING_SRCS = $(MAND_DIR)/parsing.c $(TEST_DIR)/test_parsing.c

all: $(NAME)

# Build and run the standalone parsing test harness.
parsing: $(PARSING_BIN)

$(PARSING_BIN): $(PARSING_SRCS) $(MAND_INC)/parsing.h
	$(CC) $(CFLAGS) -I $(MAND_INC) -o $(PARSING_BIN) $(PARSING_SRCS)

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
	rm -f $(PARSING_BIN)

re: fclean all

.PHONY: all parsing clean fclean re