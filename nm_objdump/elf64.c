#include "hnm.h"

/**
 * read_sym_table64 - find and read symbol table from 64-bit ELF
 * @fd: file descriptor
 * @ehdr: pointer to ELF header
 * @shdrs: array of section headers
 * @symcount: output - number of symbols found
 * @strtab_off: output - file offset of string table
 * @swap: non-zero if byte-swapping needed
 *
 * Return: allocated array of Elf64_Sym, or NULL on failure
 */
static Elf64_Sym *read_sym_table64(int fd, Elf64_Ehdr *ehdr,
		Elf64_Shdr *shdrs, size_t *symcount,
		uint64_t *strtab_off, int swap)
{
	int i, shnum, sym_idx = -1, str_idx = -1;
	Elf64_Shdr *sh;
	Elf64_Sym *syms;
	uint64_t offset, size;
	uint32_t link;
	ssize_t n;

	shnum = swap ? (int)swap16(ehdr->e_shnum) : (int)ehdr->e_shnum;
	for (i = 0; i < shnum; i++)
	{
		sh = &shdrs[i];
		uint32_t type = swap ? swap32(sh->sh_type) : sh->sh_type;

		if (type == SHT_SYMTAB)
		{
			sym_idx = i;
			link = swap ? swap32(sh->sh_link) : sh->sh_link;
			str_idx = (int)link;
			break;
		}
	}
	if (sym_idx < 0 || str_idx < 0 || str_idx >= shnum)
		return (NULL);

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
	(void)ehdr;
	return (syms);
}

/**
 * build_entries64 - build sym_entry_t array from 64-bit symbol table
 * @fd: file descriptor
 * @syms: array of Elf64_Sym
 * @shdrs: array of section headers
 * @symcount: number of symbols
 * @strtab_off: string table file offset
 * @shnum: number of sections
 * @swap: non-zero if byte-swapping needed
 *
 * Return: allocated array of sym_entry_t or NULL on failure
 */
static sym_entry_t *build_entries64(int fd, Elf64_Sym *syms,
		Elf64_Shdr *shdrs, size_t symcount,
		uint64_t strtab_off, int shnum, int swap)
{
	sym_entry_t *entries;
	size_t i;
	uint32_t name_off;
	char namebuf[256];
	ssize_t n;
	uint64_t val;

	entries = malloc(symcount * sizeof(sym_entry_t));
	if (!entries)
		return (NULL);

	for (i = 0; i < symcount; i++)
	{
		name_off = swap ? swap32(syms[i].st_name) : syms[i].st_name;
		if (lseek(fd, (off_t)(strtab_off + name_off), SEEK_SET) < 0)
		{
			entries[i].name = strdup("");
		}
		else
		{
			n = read(fd, namebuf, sizeof(namebuf) - 1);
			if (n <= 0)
				namebuf[0] = '\0';
			else
				namebuf[n] = '\0';
			namebuf[sizeof(namebuf) - 1] = '\0';
			entries[i].name = strdup(namebuf);
		}
		val = swap ? swap64(syms[i].st_value) : syms[i].st_value;
		entries[i].value = (unsigned long)val;
		entries[i].type = get_sym_type64(&syms[i], shdrs, shnum, swap);
		entries[i].has_value = (syms[i].st_shndx != SHN_UNDEF);
	}
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
	int shnum, ret = 1;
	ssize_t n;
	uint64_t shoff;
	size_t shsize;

	n = read(fd, &ehdr, sizeof(Elf64_Ehdr));
	if (n < (ssize_t)sizeof(Elf64_Ehdr))
	{
		fprintf(stderr, "hnm: %s: unexpected end of file\n", filename);
		return (1);
	}
	shnum = swap ? (int)swap16(ehdr.e_shnum) : (int)ehdr.e_shnum;
	shoff = swap ? swap64(ehdr.e_shoff) : ehdr.e_shoff;
	shsize = (size_t)shnum * sizeof(Elf64_Shdr);
	shdrs = malloc(shsize);
	if (!shdrs)
		return (1);
	if (lseek(fd, (off_t)shoff, SEEK_SET) < 0)
		goto cleanup;
	n = read(fd, shdrs, shsize);
	if (n < 0 || (size_t)n < shsize)
		goto cleanup;
	syms = read_sym_table64(fd, &ehdr, shdrs, &symcount,
				&strtab_off, swap);
	if (!syms)
	{
		free(shdrs);
		return (0);
	}
	entries = build_entries64(fd, syms, shdrs, symcount,
				strtab_off, shnum, swap);
	if (!entries)
		goto cleanup;
	print_symbols(entries, symcount);
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
