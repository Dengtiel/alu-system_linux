#include "strace.h"

/**
 * syscalls - table of x86_64 syscall numbers and names
 */
static const syscall_t syscalls[] = {
	{-1, NULL}
};

/**
 * get_syscall_name - look up a syscall name by number
 * @num: the syscall number
 *
 * Return: pointer to name string, or NULL if not found
 */
const char *get_syscall_name(long num)
{
	size_t i;

	for (i = 0; syscalls[i].name != NULL; i++)
	{
		if (syscalls[i].num == num)
			return (syscalls[i].name);
	}
	return (NULL);
}
