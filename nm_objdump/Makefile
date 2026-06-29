CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic
SRCS = hnm.c elf_dispatch.c elf32.c elf64.c sym_type.c swap.c output.c
OBJS = $(SRCS:.c=.o)
NAME = hnm

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

%.o: %.c hnm.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
