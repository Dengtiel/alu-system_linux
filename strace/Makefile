CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic

all: strace_3

strace_3: strace_3.c tracer.c syscalls.c strace.h
	$(CC) $(CFLAGS) -o strace_3 strace_3.c tracer.c syscalls.c

clean:
	rm -f strace_3

re: clean strace_3

.PHONY: all strace_3 clean re
