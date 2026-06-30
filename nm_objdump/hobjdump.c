#include "hobjdump.h"

/**
 * main - entry point for hobjdump
 * @argc: argument count
 * @argv: argument vector
 *
 * Return: 0 on success, 1 on error
 */
int main(int argc, char **argv)
{
	int i, ret = 0;

	if (argc < 2)
	{
		fprintf(stderr, "%s: no input files\n", argv[0]);
		return (1);
	}

	for (i = 1; i < argc; i++)
	{
		if (objdump_file(argv[i], argv[0]) != 0)
			ret = 1;
	}

	return (ret);
}
