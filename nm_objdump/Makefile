CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic

HNM_SRCS = hnm.c elf_dispatch.c elf32.c elf32_utils.c elf64.c elf64_utils.c \
	sym_type.c swap.c output.c
HNM_OBJS = $(HNM_SRCS:.c=.o)

HOBJ_SRCS = hobjdump.c dispatch.c obj32.c obj64.c elfinfo.c hexdump.c bswap.c
HOBJ_OBJS = $(HOBJ_SRCS:.c=.o)

all: hnm hobjdump

hnm: $(HNM_OBJS)
	$(CC) $(CFLAGS) -o hnm $(HNM_OBJS)

hobjdump: $(HOBJ_OBJS)
	$(CC) $(CFLAGS) -o hobjdump $(HOBJ_OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(HNM_OBJS) $(HOBJ_OBJS)

fclean: clean
	rm -f hnm hobjdump

re: fclean all

.PHONY: all hnm hobjdump clean fclean re
