CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic

all: todo_api_0 todo_api_1 todo_api_2

todo_api_0: todo_api_0.c
	$(CC) $(CFLAGS) -o $@ $<

todo_api_1: todo_api_1.c
	$(CC) $(CFLAGS) -o $@ $<

todo_api_2: todo_api_2.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f todo_api_0 todo_api_1 todo_api_2

.PHONY: all clean
