#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include "syscall_table.h"

/**
 * print_syscall - print syscall name, args and return value
 * @entry: registers at syscall entry
 * @retval: syscall return value
 */
static void print_syscall(struct user_regs_struct *entry,
		unsigned long retval)
{
	long num = (long)entry->orig_rax;
	const char *name;
	int nparams;
	unsigned long args[6];
	int i;

	if (num < 0 || num >= SYSCALL_TABLE_SIZE)
	{
		name = "unknown";
		nparams = 6;
	}
	else
	{
		name = syscall_table[num].name;
		nparams = syscall_table[num].nparams;
	}

	args[0] = entry->rdi;
	args[1] = entry->rsi;
	args[2] = entry->rdx;
	args[3] = entry->r10;
	args[4] = entry->r8;
	args[5] = entry->r9;

	if (nparams == -1)
	{
		printf("%s(0x%lx, 0x%lx, ...)", name, args[0], args[1]);
	}
	else if (nparams == 0)
	{
		printf("%s()", name);
	}
	else
	{
		printf("%s(", name);
		for (i = 0; i < nparams; i++)
		{
			if (i > 0)
				printf(", ");
			printf("0x%lx", args[i]);
		}
		printf(")");
	}

	if (num == 231 || num == 60)
		printf(" = ?\n");
	else
		printf(" = 0x%lx\n", retval);
}

/**
 * main - trace a program using ptrace
 * @argc: argument count
 * @argv: argument vector
 *
 * Return: 0 on success, 1 on error
 */
int main(int argc, char **argv)
{
	pid_t child;
	int status;
	struct user_regs_struct regs, entry_regs;
	int in_syscall = 0;
	int execve_done = 0;

	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s command [args...]\n", argv[0]);
		return (1);
	}

	child = fork();
	if (child < 0)
	{
		perror("fork");
		return (1);
	}

	if (child == 0)
	{
		ptrace(PTRACE_TRACEME, 0, NULL, NULL);
		execv(argv[1], argv + 1);
		perror("execv");
		exit(1);
	}

	while (1)
	{
		wait(&status);
		if (WIFEXITED(status) || WIFSIGNALED(status))
			break;
		if (!WIFSTOPPED(status))
			continue;

		ptrace(PTRACE_GETREGS, child, NULL, &regs);

		if (!execve_done)
		{
			printf("execve(0x0, 0x0, 0x0) = 0x0\n");
			execve_done = 1;
			ptrace(PTRACE_SYSCALL, child, NULL, NULL);
			continue;
		}

		if (!in_syscall)
		{
			entry_regs = regs;
			in_syscall = 1;
		}
		else
		{
			in_syscall = 0;
			print_syscall(&entry_regs, (unsigned long)regs.rax);
		}

		ptrace(PTRACE_SYSCALL, child, NULL, NULL);
	}

	return (0);
}
