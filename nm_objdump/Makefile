CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic
SRCS = hnm.c elf_dispatch.c elf32.c elf32_utils.c elf64.c elf64_utils.c \
	sym_type.c swap.c output.c
OBJS = $(SRCS:.c=.o)
NAME = hnm

all: hnm

hnm: $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

%.o: %.c hnm.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean hnm

.PHONY: all hnm clean fclean re
