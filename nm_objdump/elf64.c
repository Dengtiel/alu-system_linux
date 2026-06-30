#include "hnm.h"

/**
 * read_sym_table64 - read symbol table from 64-bit ELF
 * @fd: file descriptor
 * @shdrs: array of section headers
 * @sym_idx: symbol table section index
 * @str_idx: string table section index
 * @symcount: output number of symbols
 * @strtab_off: output string table file offset
 * @swap: non-zero if byte-swapping needed
 *
 * Return: allocated Elf64_Sym array or NULL
 */
static Elf64_Sym *read_sym_table64(int fd, Elf64_Shdr *shdrs,
		int sym_idx, int str_idx, size_t *symcount,
		uint64_t *strtab_off, int swap)
{
	uint64_t offset, size;
	Elf64_Sym *syms;
	ssize_t n;

	offset = swap ? swap64(shdrs[sym_idx].sh_offset)
		: shdrs[sym_idx].sh_offset;
	size = swap ? swap64(shdrs[sym_idx].sh_size)
		: shdrs[sym_idx].sh_size;
	*strtab_off = swap ? swap64(shdrs[str_idx].sh_offset)
		: shdrs[str_idx].sh_offset;
	*symcount = (size_t)(size / sizeof(Elf64_Sym));
	syms = malloc((size_t)size);
	if (!syms)
		return (NULL);
	if (lseek(fd, (off_t)offset, SEEK_SET) < 0)
	{
		free(syms);
		return (NULL);
	}
	n = read(fd, syms, (size_t)size);
	if (n < 0 || (size_t)n < (size_t)size)
	{
		free(syms);
		return (NULL);
	}
	return (syms);
}

/**
 * read_elf64_core - core logic for reading 64-bit ELF
 * @fd: file descriptor
 * @filename: file name for errors
 * @prog: program name for errors
 * @swap: non-zero if byte-swapping needed
 *
 * Return: 0 on success, 1 on error
 */
static int read_elf64_core(int fd, const char *filename,
		const char *prog, int swap)
{
	Elf64_Ehdr ehdr;
	Elf64_Shdr *shdrs = NULL;
	Elf64_Sym *syms = NULL;
	sym_entry_t *entries = NULL;
	size_t symcount = 0;
	uint64_t strtab_off = 0;
	int shnum, sym_idx, str_idx, ret = 1;
	ssize_t n;

	n = read(fd, &ehdr, sizeof(Elf64_Ehdr));
	if (n < (ssize_t)sizeof(Elf64_Ehdr))
		return (1);
	shnum = swap ? (int)swap16(ehdr.e_shnum) : (int)ehdr.e_shnum;
	shdrs = load_shdrs64(fd, &ehdr, shnum, swap);
	if (!shdrs)
		return (1);
	find_symtab64(shdrs, shnum, &sym_idx, &str_idx, swap);
	if (sym_idx < 0 || str_idx < 0 || str_idx >= shnum)
	{
		fprintf(stderr, "%s: %s: no symbols\n", prog, filename);
		free(shdrs);
		return (0);
	}
	syms = read_sym_table64(fd, shdrs, sym_idx, str_idx,
				&symcount, &strtab_off, swap);
	if (!syms)
		goto cleanup;
	entries = build_entries64(fd, syms, shdrs, symcount,
				strtab_off, shnum, swap);
	if (!entries)
		goto cleanup;
	print_symbols(entries, symcount, 1);
	free_symbols(entries, symcount);
	ret = 0;
cleanup:
	free(shdrs);
	free(syms);
	return (ret);
}

/**
 * read_elf64_le - read little-endian 64-bit ELF
 * @fd: file descriptor
 * @filename: file name for errors
 * @prog: program name for errors
 *
 * Return: 0 on success, 1 on error
 */
int read_elf64_le(int fd, const char *filename, const char *prog)
{
	return (read_elf64_core(fd, filename, prog, 0));
}

/**
 * read_elf64_be - read big-endian 64-bit ELF
 * @fd: file descriptor
 * @filename: file name for errors
 * @prog: program name for errors
 *
 * Return: 0 on success, 1 on error
 */
int read_elf64_be(int fd, const char *filename, const char *prog)
{
	return (read_elf64_core(fd, filename, prog, 1));
}
