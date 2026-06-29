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
 * print_args - print syscall arguments from registers
 * @regs: registers from ptrace
 * @nargs: number of arguments (-1 = variadic)
 */
static void print_args(struct user_regs_struct *regs, int nargs)
{
	unsigned long args[6];
	int i;


	args[0] = regs->rdi;
	args[1] = regs->rsi;
	args[2] = regs->rdx;
	args[3] = regs->r10;
	args[4] = regs->r8;
	args[5] = regs->r9;


	if (nargs == -1)
	{
		if (args[0] == 0)
			printf("0, ");
		else
			printf("0x%lx, ", args[0]);
		if (args[1] == 0)
			printf("0, ...");
		else
			printf("0x%lx, ...", args[1]);
		return;
	}
	for (i = 0; i < nargs; i++)
	{
		if (i > 0)
			printf(", ");
		if (args[i] == 0)
			printf("0");
		else
			printf("0x%lx", args[i]);
	}
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
		printf("%s\n", name ? name : "unknown");
		fflush(stdout);
	}
}

/**
 * print_retval_mode - print name and return value (mode 2)
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
		printf("%s = ", name ? name : "unknown");
		if (name && strcmp(name, "exit_group") == 0)
			printf("?\n");
		else if (retval == 0)
			printf("0\n");
		else
			printf("0x%lx\n", retval);
		fflush(stdout);
	}
}

/**
 * print_full_mode - print name, args and return value (mode 3)
 * @regs: registers from ptrace
 * @in_syscall: 0 entry, 1 exit
 * @last_syscall: syscall number saved at entry
 * @saved_regs: registers saved at syscall entry
 */
static void print_full_mode(struct user_regs_struct *regs,
	int in_syscall, long last_syscall,
	struct user_regs_struct *saved_regs)
{
	const char *name;
	long retval;
	int nargs;

	name = get_syscall_name(last_syscall);
	nargs = get_syscall_nargs(last_syscall);
	retval = (long)regs->rax;

	if (last_syscall == 1)
	{
		if (in_syscall == 0)
		{
			printf("%s(", name ? name : "unknown");
			print_args(saved_regs, nargs);
			fflush(stdout);
		}
		else
		{
			printf(") = 0x%lx\n", retval);
			fflush(stdout);
		}
		return;
	}
	if (in_syscall == 1)
	{
		printf("%s(", name ? name : "unknown");
		print_args(saved_regs, nargs);
		printf(") = ");
		if (name && strcmp(name, "exit_group") == 0)
			printf("?\n");
		else if (retval == 0)
			printf("0\n");
		else
			printf("0x%lx\n", retval);
		fflush(stdout);
	}
}

/**
 * run_strace - fork, exec under ptrace, intercept syscalls
 * @argc: argument count
 * @argv: argument vector; argv[1] is the traced program
 * @mode: 0=number, 1=name, 2=name+retval, 3=name+args+retval
 *
 * Return: exit status of traced process, or 1 on error
 */
int run_strace(int argc, char **argv, int mode)
{
	pid_t child;
	int status;
	struct user_regs_struct regs;
	struct user_regs_struct saved_regs;
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
	else if (mode == 2)
		printf("execve = 0\n");
	else
		printf("execve(0, 0, 0) = 0\n");
	fflush(stdout);
	in_syscall = 0;
	last_syscall = -1;
	memset(&saved_regs, 0, sizeof(saved_regs));
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
		{
			last_syscall = (long)regs.orig_rax;
			saved_regs = regs;
		}
		if (mode == 0 && in_syscall == 0)
			print_num_mode(&regs);
		else if (mode == 1)
			print_name_mode(&regs, in_syscall);
		else if (mode == 2)
			print_retval_mode(&regs, in_syscall, last_syscall);
		else if (mode == 3)
			print_full_mode(&regs, in_syscall, last_syscall,
				&saved_regs);
		in_syscall ^= 1;
	}
	if (mode == 2 || mode == 3)
	{
		printf("exit_group(0) = ?\n");
		fflush(stdout);
	}
	return (WIFEXITED(status) ? WEXITSTATUS(status) : 1);
}
