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
 * print_syscall_num - print syscall number from registers
 * @regs: pointer to user_regs_struct filled by ptrace
 */
static void print_syscall_num(struct user_regs_struct *regs)
{
	printf("%lu\n", (unsigned long)regs->orig_rax);
	fflush(stdout);
}

/**
 * print_syscall_name_regs - print syscall name from registers
 * @regs: pointer to user_regs_struct filled by ptrace
 * @in_syscall: 0 = entry, 1 = exit
 */
static void print_syscall_name_regs(struct user_regs_struct *regs,
	int in_syscall)
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
 * run_strace - fork, exec under ptrace, intercept syscalls
 * @argc: argument count
 * @argv: argument vector; argv[1] is the traced program
 * @print_name: 0 for number, 1 for name
 *
 * Return: exit status of traced process, or 1 on error
 */
int run_strace(int argc, char **argv, int print_name)
{
	pid_t child;
	int status;
	struct user_regs_struct regs;
	int in_syscall;

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

	if (print_name)
		printf("execve\n");
	else
		printf("59\n");
	fflush(stdout);

	in_syscall = 0;
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

		if (print_name)
			print_syscall_name_regs(&regs, in_syscall);
		else if (in_syscall == 0)
			print_syscall_num(&regs);

		in_syscall ^= 1;
	}
	return (WIFEXITED(status) ? WEXITSTATUS(status) : 1);
}
