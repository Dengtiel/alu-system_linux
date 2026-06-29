CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic
OBJS_COMMON = tracer.o syscalls.o

all: strace_0 strace_1

strace_0: strace_0.o $(OBJS_COMMON)
	$(CC) $(CFLAGS) -o $@ $^

strace_1: strace_1.o $(OBJS_COMMON)
	$(CC) $(CFLAGS) -o $@ $^

strace_0.o: strace_0.c strace.h
	$(CC) $(CFLAGS) -c -o $@ $<

strace_1.o: strace_1.c strace.h
	$(CC) $(CFLAGS) -c -o $@ $<

tracer.o: tracer.c strace.h
	$(CC) $(CFLAGS) -c -o $@ $<

syscalls.o: syscalls.c strace.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o strace_0 strace_1

.PHONY: all clean
