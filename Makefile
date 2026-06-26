NAME    = ft_ping

CC      = cc
CFLAGS  = -Wall -Wextra -Werror

SRC_DIR = mandatory
OBJ_DIR = obj
INC_DIR = $(SRC_DIR)/includes
INCLUDES= -I $(INC_DIR)
LDFLAGS = -lm

SRCS    =	main.c			\
			parsing.c		\
			dns.c			\
			socket.c		\
			in_checksum.c	\
			icmp.c			\
			ping.c

OBJS    = $(SRCS:%.c=$(OBJ_DIR)/%.o)

# ── bonus build ──────────────────────────────────────────────────────
BONUS_NAME    = ft_ping_bonus
BONUS_DIR     = bonuses
BONUS_INC     = $(BONUS_DIR)/includes
BONUS_OBJ_DIR = obj_bonus
BONUS_SRCS    = $(SRCS)
BONUS_OBJS    = $(BONUS_SRCS:%.c=$(BONUS_OBJ_DIR)/%.o)

# ── parsing unit test ───────────────────────────────────────────────
TEST_DIR     = $(SRC_DIR)/test
PARSING_BIN  = $(TEST_DIR)/test_parsing
PARSING_SRCS = $(SRC_DIR)/parsing.c $(TEST_DIR)/test_parsing.c

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Build the standalone parsing test harness.
parsing: $(PARSING_BIN)

$(PARSING_BIN): $(PARSING_SRCS) $(INC_DIR)/parsing.h
	$(CC) $(CFLAGS) $(INCLUDES) -o $(PARSING_BIN) $(PARSING_SRCS)

# Build the bonus binary from the bonuses/ source tree.
bonus: $(BONUS_NAME)

$(BONUS_NAME): $(BONUS_OBJS)
	$(CC) $(CFLAGS) -o $(BONUS_NAME) $(BONUS_OBJS) $(LDFLAGS)

$(BONUS_OBJ_DIR)/%.o: $(BONUS_DIR)/%.c | $(BONUS_OBJ_DIR)
	$(CC) $(CFLAGS) -I $(BONUS_INC) -c $< -o $@

$(BONUS_OBJ_DIR):
	mkdir -p $(BONUS_OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR)
	rm -rf $(BONUS_OBJ_DIR)

fclean: clean
	rm -f $(NAME)
	rm -f $(BONUS_NAME)
	rm -f $(PARSING_BIN)

re: fclean all

.PHONY: all bonus parsing clean fclean re
