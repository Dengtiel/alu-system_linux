#include "strace.h"

/**
 * main - entry point for strace_1 (print syscall names)
 * @argc: argument count
 * @argv: argument vector
 *
 * Return: exit status of traced process, or 1 on error
 */
int main(int argc, char **argv)
{
	return (run_strace(argc, argv, 1));
}
