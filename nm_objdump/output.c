#include "hnm.h"

/**
 * print_symbols - print symbol table in nm -p format
 * @syms: array of sym_entry_t
 * @count: number of entries
 * @is64: 1 if 64-bit ELF, 0 if 32-bit
 */
void print_symbols(sym_entry_t *syms, size_t count, int is64)
{
	size_t i;

	for (i = 0; i < count; i++)
	{
		if (syms[i].name == NULL || syms[i].name[0] == '\0')
			continue;
		if (syms[i].type == 0)
			continue;
		if (syms[i].has_value)
		{
			if (is64)
				printf("%016lx %c %s\n",
					syms[i].value,
					syms[i].type,
					syms[i].name);
			else
				printf("%08lx %c %s\n",
					syms[i].value,
					syms[i].type,
					syms[i].name);
		}
		else
		{
			if (is64)
				printf("                 %c %s\n",
					syms[i].type,
					syms[i].name);
			else
				printf("         %c %s\n",
					syms[i].type,
					syms[i].name);
		}
	}
}

/**
 * free_symbols - free memory used by symbol array
 * @syms: array of sym_entry_t
 * @count: number of entries
 */
void free_symbols(sym_entry_t *syms, size_t count)
{
	size_t i;

	if (!syms)
		return;
	for (i = 0; i < count; i++)
	{
		if (syms[i].name)
			free(syms[i].name);
	}
	free(syms);
}
