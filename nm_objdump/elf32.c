#include "hnm.h"

/**
 * find_symtab32 - find symbol table and string table indices
 * @shdrs: array of section headers
 * @shnum: number of sections
 * @sym_idx: output symbol table index
 * @str_idx: output string table index
 * @swap: non-zero if byte-swapping needed
 */
static void find_symtab32(Elf32_Shdr *shdrs, int shnum,
		int *sym_idx, int *str_idx, int swap)
{
	int i;
	uint32_t type, link;

	*sym_idx = -1;
	*str_idx = -1;
	for (i = 0; i < shnum; i++)
	{
		type = swap ? swap32(shdrs[i].sh_type) : shdrs[i].sh_type;
		if (type == SHT_SYMTAB)
		{
			*sym_idx = i;
			link = swap ? swap32(shdrs[i].sh_link)
				: shdrs[i].sh_link;
			*str_idx = (int)link;
			break;
		}
	}
}

/**
 * read_sym_table32 - read symbol table from 32-bit ELF
 * @fd: file descriptor
 * @shdrs: array of section headers
 * @sym_idx: symbol table section index
 * @str_idx: string table section index
 * @symcount: output number of symbols
 * @strtab_off: output string table file offset
 * @swap: non-zero if byte-swapping needed
 *
 * Return: allocated Elf32_Sym array or NULL
 */
static Elf32_Sym *read_sym_table32(int fd, Elf32_Shdr *shdrs,
		int sym_idx, int str_idx, size_t *symcount,
		uint32_t *strtab_off, int swap)
{
	uint32_t offset, size;
	Elf32_Sym *syms;
	ssize_t n;

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
	return (syms);
}

/**
 * build_entry32 - build one sym_entry_t from a 32-bit symbol
 * @fd: file descriptor
 * @sym: pointer to Elf32_Sym
 * @shdrs: array of section headers
 * @strtab_off: string table file offset
 * @shnum: number of sections
 * @swap: non-zero if byte-swapping needed
 *
 * Return: populated sym_entry_t
 */
static sym_entry_t build_entry32(int fd, Elf32_Sym *sym,
		Elf32_Shdr *shdrs, uint32_t strtab_off,
		int shnum, int swap)
{
	sym_entry_t entry;
	uint32_t name_off, val;
	char namebuf[256];
	ssize_t n;

	name_off = swap ? swap32(sym->st_name) : sym->st_name;
	if (lseek(fd, strtab_off + name_off, SEEK_SET) < 0)
	{
		entry.name = strdup("");
	}
	else
	{
		n = read(fd, namebuf, sizeof(namebuf) - 1);
		namebuf[n > 0 ? n : 0] = '\0';
		namebuf[sizeof(namebuf) - 1] = '\0';
		entry.name = strdup(namebuf);
	}
	val = swap ? swap32(sym->st_value) : sym->st_value;
	entry.value = (unsigned long)val;
	entry.type = get_sym_type32(sym, shdrs, shnum, swap);
	entry.has_value = (sym->st_shndx != SHN_UNDEF);
	return (entry);
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
	size_t symcount = 0, i;
	uint32_t strtab_off = 0;
	int shnum, sym_idx, str_idx, ret = 1;
	ssize_t n;

	n = read(fd, &ehdr, sizeof(Elf32_Ehdr));
	if (n < (ssize_t)sizeof(Elf32_Ehdr))
		return (1);
	shnum = swap ? (int)swap16(ehdr.e_shnum) : (int)ehdr.e_shnum;
	shdrs = malloc((size_t)shnum * sizeof(Elf32_Shdr));
	if (!shdrs)
		return (1);
	if (lseek(fd, swap ? swap32(ehdr.e_shoff) : ehdr.e_shoff,
		SEEK_SET) < 0)
		goto cleanup;
	n = read(fd, shdrs, (size_t)shnum * sizeof(Elf32_Shdr));
	if (n < 0 || (size_t)n < (size_t)shnum * sizeof(Elf32_Shdr))
		goto cleanup;
	find_symtab32(shdrs, shnum, &sym_idx, &str_idx, swap);
	if (sym_idx < 0 || str_idx < 0 || str_idx >= shnum)
	{
		fprintf(stderr, "hnm: %s: no symbols\n", filename);
		free(shdrs);
		return (0);
	}
	syms = read_sym_table32(fd, shdrs, sym_idx, str_idx,
				&symcount, &strtab_off, swap);
	if (!syms)
		goto cleanup;
	entries = malloc(symcount * sizeof(sym_entry_t));
	if (!entries)
		goto cleanup;
	for (i = 0; i < symcount; i++)
		entries[i] = build_entry32(fd, &syms[i], shdrs,
					strtab_off, shnum, swap);
	print_symbols(entries, symcount, 0);
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
