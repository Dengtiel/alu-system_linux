#include "hnm.h"

/**
 * read_sym_table32 - find and read symbol table from 32-bit ELF
 * @fd: file descriptor (seeked to start)
 * @ehdr: pointer to ELF header
 * @shdrs: array of section headers
 * @symcount: output - number of symbols found
 * @strtab_off: output - file offset of string table
 * @swap: non-zero if byte-swapping needed
 *
 * Return: allocated array of Elf32_Sym, or NULL on failure
 */
static Elf32_Sym *read_sym_table32(int fd, Elf32_Ehdr *ehdr,
		Elf32_Shdr *shdrs, size_t *symcount,
		uint32_t *strtab_off, int swap)
{
	int i, shnum, sym_idx = -1, str_idx = -1;
	Elf32_Shdr *sh;
	Elf32_Sym *syms;
	uint32_t offset, size, link;
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

	offset = swap ? swap32(shdrs[sym_idx].sh_offset)
		: shdrs[sym_idx].sh_offset;
	size = swap ? swap32(shdrs[sym_idx].sh_size)
		: shdrs[sym_idx].sh_size;
	*strtab_off = swap ? swap32(shdrs[str_idx].sh_offset)
		: shdrs[str_idx].sh_offset;
	*symcount = size / sizeof(Elf32_Sym);

	syms = malloc(size);
	if (!syms)
		return (NULL);
	if (lseek(fd, offset, SEEK_SET) < 0)
	{
		free(syms);
		return (NULL);
	}
	n = read(fd, syms, size);
	if (n < 0 || (size_t)n < size)
	{
		free(syms);
		return (NULL);
	}
	(void)ehdr;
	return (syms);
}

/**
 * build_entries32 - build sym_entry_t array from 32-bit symbol table
 * @fd: file descriptor
 * @syms: array of Elf32_Sym
 * @shdrs: array of section headers
 * @symcount: number of symbols
 * @strtab_off: string table file offset
 * @shnum: number of sections
 * @swap: non-zero if byte-swapping needed
 *
 * Return: allocated array of sym_entry_t or NULL on failure
 */
static sym_entry_t *build_entries32(int fd, Elf32_Sym *syms,
		Elf32_Shdr *shdrs, size_t symcount,
		uint32_t strtab_off, int shnum, int swap)
{
	sym_entry_t *entries;
	size_t i;
	uint32_t name_off;
	char namebuf[256];
	ssize_t n;
	uint32_t val;

	entries = malloc(symcount * sizeof(sym_entry_t));
	if (!entries)
		return (NULL);

	for (i = 0; i < symcount; i++)
	{
		name_off = swap ? swap32(syms[i].st_name) : syms[i].st_name;
		if (lseek(fd, strtab_off + name_off, SEEK_SET) < 0)
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
		val = swap ? swap32(syms[i].st_value) : syms[i].st_value;
		entries[i].value = (unsigned long)val;
		entries[i].type = get_sym_type32(&syms[i], shdrs, shnum, swap);
		entries[i].has_value = (syms[i].st_shndx != SHN_UNDEF);
	}
	return (entries);
}

/**
 * read_elf32_core - core logic for reading 32-bit ELF
 * @fd: file descriptor
 * @filename: file name for errors
 * @swap: non-zero if byte-swapping needed
 *
 * Return: 0 on success, 1 on error
 */
static int read_elf32_core(int fd, const char *filename, int swap)
{
	Elf32_Ehdr ehdr;
	Elf32_Shdr *shdrs = NULL;
	Elf32_Sym *syms = NULL;
	sym_entry_t *entries = NULL;
	size_t symcount = 0;
	uint32_t strtab_off = 0;
	int shnum, ret = 1;
	ssize_t n;
	uint32_t shoff;
	size_t shsize;

	n = read(fd, &ehdr, sizeof(Elf32_Ehdr));
	if (n < (ssize_t)sizeof(Elf32_Ehdr))
	{
		fprintf(stderr, "hnm: %s: unexpected end of file\n", filename);
		return (1);
	}
	shnum = swap ? (int)swap16(ehdr.e_shnum) : (int)ehdr.e_shnum;
	shoff = swap ? swap32(ehdr.e_shoff) : ehdr.e_shoff;
	shsize = (size_t)shnum * sizeof(Elf32_Shdr);
	shdrs = malloc(shsize);
	if (!shdrs)
		return (1);
	if (lseek(fd, shoff, SEEK_SET) < 0)
		goto cleanup;
	n = read(fd, shdrs, shsize);
	if (n < 0 || (size_t)n < shsize)
		goto cleanup;
	syms = read_sym_table32(fd, &ehdr, shdrs, &symcount,
				&strtab_off, swap);
	if (!syms)
	{
		free(shdrs);
		return (0);
	}
	entries = build_entries32(fd, syms, shdrs, symcount,
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
 * read_elf32_le - read little-endian 32-bit ELF
 * @fd: file descriptor
 * @filename: file name for errors
 *
 * Return: 0 on success, 1 on error
 */
int read_elf32_le(int fd, const char *filename)
{
	return (read_elf32_core(fd, filename, 0));
}

/**
 * read_elf32_be - read big-endian 32-bit ELF
 * @fd: file descriptor
 * @filename: file name for errors
 *
 * Return: 0 on success, 1 on error
 */
int read_elf32_be(int fd, const char *filename)
{
	return (read_elf32_core(fd, filename, 1));
}
