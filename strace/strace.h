#ifndef STRACE_H
#define STRACE_H

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

/**
 * struct syscall_s - maps a syscall number to its name
 * @num: syscall number
 * @name: syscall name string
 */
typedef struct syscall_s
{
	long num;
	const char *name;
} syscall_t;

const char *get_syscall_name(long num);
int run_strace(int argc, char **argv, int print_name);

#endif /* STRACE_H */
