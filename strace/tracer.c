#include "strace.h"

/**
 * child_exec - run the target command under ptrace
 * @argv: argument vector; argv[0] is the program path
 */
static void child_exec(char **argv)
{
	if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) == -1)
	{
		perror("ptrace(TRACEME)");
		exit(EXIT_FAILURE);
	}
	execv(argv[0], argv);
	perror("execv");
	exit(EXIT_FAILURE);
}

/**
 * print_num_mode - print syscall number mode
 * @regs: registers from ptrace
 */
static void print_num_mode(struct user_regs_struct *regs)
{
	printf("%lu\n", (unsigned long)regs->orig_rax);
	fflush(stdout);
}

/**
 * print_name_mode - print syscall name mode
 * @regs: registers from ptrace
 * @in_syscall: 0 entry, 1 exit
 */
static void print_name_mode(struct user_regs_struct *regs, int in_syscall)
{
	long num;
	const char *name;

	num = (long)regs->orig_rax;
	name = get_syscall_name(num);
	if (num == 1)
	{
		if (in_syscall == 0)
		{
			printf("%s", name ? name : "unknown");
			fflush(stdout);
		}
		else
		{
			printf("\n");
			fflush(stdout);
		}
		return;
	}
	if (in_syscall == 0)
	{
		if (name != NULL)
			printf("%s\n", name);
		else
			printf("unknown_%ld\n", num);
		fflush(stdout);
	}
}

/**
 * print_retval_mode - print syscall name and return value
 * @regs: registers from ptrace
 * @in_syscall: 0 entry, 1 exit
 * @last_syscall: syscall number saved at entry
 */
static void print_retval_mode(struct user_regs_struct *regs,
	int in_syscall, long last_syscall)
{
	const char *name;
	long retval;

	name = get_syscall_name(last_syscall);
	retval = (long)regs->rax;

	if (last_syscall == 1)
	{
		if (in_syscall == 0)
		{
			printf("%s", name ? name : "unknown");
			fflush(stdout);
		}
		else
		{
			printf(" = 0x%lx\n", retval);
			fflush(stdout);
		}
		return;
	}
	if (in_syscall == 1)
	{
		if (name != NULL)
			printf("%s = ", name);
		else
			printf("unknown_%ld = ", last_syscall);
		if (name && strcmp(name, "exit_group") == 0)
			printf("?\n");
		else
			printf("0x%lx\n", retval);
		fflush(stdout);
	}
}

/**
 * run_strace - fork, exec under ptrace, intercept syscalls
 * @argc: argument count
 * @argv: argument vector; argv[1] is the traced program
 * @mode: 0=number, 1=name, 2=name+retval
 *
 * Return: exit status of traced process, or 1 on error
 */
int run_strace(int argc, char **argv, int mode)
{
	pid_t child;
	int status;
	struct user_regs_struct regs;
	int in_syscall;
	long last_syscall;

	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s command [args...]\n", argv[0]);
		return (1);
	}
	child = fork();
	if (child == -1)
	{
		perror("fork");
		return (1);
	}
	if (child == 0)
		child_exec(argv + 1);

	waitpid(child, &status, 0);
	ptrace(PTRACE_SETOPTIONS, child, 0, PTRACE_O_TRACESYSGOOD);

	if (mode == 1)
		printf("execve\n");
	else if (mode == 0)
		printf("59\n");
	else
		printf("execve = 0\n");
	fflush(stdout);

	in_syscall = 0;
	last_syscall = -1;
	while (1)
	{
		ptrace(PTRACE_SYSCALL, child, NULL, NULL);
		waitpid(child, &status, 0);

		if (WIFEXITED(status) || WIFSIGNALED(status))
			break;

		if (!WIFSTOPPED(status))
			continue;

		if ((WSTOPSIG(status) & 0x80) == 0)
			continue;

		ptrace(PTRACE_GETREGS, child, NULL, &regs);

		if (in_syscall == 0)
			last_syscall = (long)regs.orig_rax;

		if (mode == 0 && in_syscall == 0)
			print_num_mode(&regs);
		else if (mode == 1)
			print_name_mode(&regs, in_syscall);
		else if (mode == 2)
			print_retval_mode(&regs, in_syscall, last_syscall);

		in_syscall ^= 1;
	}
	/* print exit_group = ? */
	if (mode == 2)
	{
		printf("exit_group = ?\n");
		fflush(stdout);
	}
	return (WIFEXITED(status) ? WEXITSTATUS(status) : 1);
}
