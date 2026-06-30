CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic

all: strace_3

strace_3: strace_3.c syscall_table.h
	$(CC) $(CFLAGS) -o strace_3 strace_3.c

clean:
	rm -f strace_3

re: clean strace_3

.PHONY: all clean re
