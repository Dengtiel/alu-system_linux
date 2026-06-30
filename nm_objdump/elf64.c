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
 * build_entry64 - build one sym_entry_t from a 64-bit symbol
 * @fd: file descriptor
 * @sym: pointer to Elf64_Sym
 * @shdrs: array of section headers
 * @strtab_off: string table file offset
 * @shnum: number of sections
 * @swap: non-zero if byte-swapping needed
 *
 * Return: populated sym_entry_t
 */
static sym_entry_t build_entry64(int fd, Elf64_Sym *sym,
		Elf64_Shdr *shdrs, uint64_t strtab_off,
		int shnum, int swap)
{
	sym_entry_t entry;
	uint32_t name_off;
	uint64_t val;
	char namebuf[256];
	ssize_t n;

	name_off = swap ? swap32(sym->st_name) : sym->st_name;
	if (lseek(fd, (off_t)(strtab_off + name_off), SEEK_SET) < 0)
		entry.name = strdup("");
	else
	{
		n = read(fd, namebuf, sizeof(namebuf) - 1);
		namebuf[n > 0 ? n : 0] = '\0';
		entry.name = strdup(namebuf);
	}
	val = swap ? swap64(sym->st_value) : sym->st_value;
	entry.value = (unsigned long)val;
	entry.type = get_sym_type64(sym, shdrs, shnum, swap);
	entry.has_value = (sym->st_shndx != SHN_UNDEF);
	return (entry);
}

/**
 * build_entries64 - build full sym_entry_t array for 64-bit ELF
 * @fd: file descriptor
 * @syms: array of Elf64_Sym
 * @shdrs: array of section headers
 * @symcount: number of symbols
 * @strtab_off: string table file offset
 * @shnum: number of sections
 * @swap: non-zero if byte-swapping needed
 *
 * Return: allocated sym_entry_t array or NULL
 */
static sym_entry_t *build_entries64(int fd, Elf64_Sym *syms,
		Elf64_Shdr *shdrs, size_t symcount,
		uint64_t strtab_off, int shnum, int swap)
{
	sym_entry_t *entries;
	size_t i;

	entries = malloc(symcount * sizeof(sym_entry_t));
	if (!entries)
		return (NULL);
	for (i = 0; i < symcount; i++)
		entries[i] = build_entry64(fd, &syms[i], shdrs,
					strtab_off, shnum, swap);
	return (entries);
}

/**
 * read_elf64_core - core logic for reading 64-bit ELF
 * @fd: file descriptor
 * @filename: file name for errors
 * @swap: non-zero if byte-swapping needed
 *
 * Return: 0 on success, 1 on error
 */
static int read_elf64_core(int fd, const char *filename, int swap)
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
		fprintf(stderr, "hnm: %s: no symbols\n", filename);
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
 *
 * Return: 0 on success, 1 on error
 */
int read_elf64_le(int fd, const char *filename)
{
	return (read_elf64_core(fd, filename, 0));
}

/**
 * read_elf64_be - read big-endian 64-bit ELF
 * @fd: file descriptor
 * @filename: file name for errors
 *
 * Return: 0 on success, 1 on error
 */
int read_elf64_be(int fd, const char *filename)
{
	return (read_elf64_core(fd, filename, 1));
}
